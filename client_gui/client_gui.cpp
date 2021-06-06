#include "client_gui.h"

#include <algorithm>
#include <execution>
#include <array>
#include <atomic>

#include "imgui.h"
#include "cpr/cpr.h"
#include "nlohmann/json.hpp"

#include <Magnum/Math/Color.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Trade/MeshData.h>

#include "ZSerializer.hpp"

#include "../client_core/Login.h"
#include "../common/Position.h"
#include "../common/Velocity.h"
#include "../common/Name.h"
#include "../common/Math.h"
#include "../common/UUID.h"

using namespace ct;
using namespace Math::Literals;
using asio::ip::tcp;
using json = nlohmann::json;

namespace
{
    std::vector<std::string> logs;
    void Log(const std::string& str)
    {
        logs.push_back(str);
    }

    struct ConnectionID
    {
        uint64_t id;
    };

    tcp::endpoint StringToEndpoint(const std::string& str)
    {
        auto pos = str.find(':');
        if (pos == std::string::npos)
            return {};
        auto ip = str.substr(0, pos);
        int port = std::stoi(str.substr(pos + 1));
        return { asio::ip::make_address_v4(ip), (unsigned short)port };
    }

    template<typename In>
    bool ReadEntity(In& in, EntityHandle e)
    {
        auto uuid = zs::Read<UUID>(in);
        if (std::holds_alternative<zs::Error>(uuid))
            return false;
        *e.Add<UUID>() = std::get<0>(uuid);

        zs::StringReader components(std::get<0>(zs::Read<std::string>(in)));
        while (true)
        {
            auto _tag = zs::Read<std::string>(components);
            if (std::holds_alternative<zs::Error>(_tag))
                break;

            auto tag = std::get<0>(_tag);
            if (tag == "connection")
            {
                if (!e.Has<ConnectionID>())
                    e.Add<ConnectionID>();
                *e.Get<ConnectionID>() = std::get<0>(zs::Read<ConnectionID>(components));
            }
            else if (tag == "position")
            {
                if (!e.Has<Position>())
                    e.Add<Position>();
                *e.Get<Position>() = std::get<0>(zs::Read<Position>(components));
            }
            else if (tag == "velocity")
            {
                if (!e.Has<Velocity>())
                    e.Add<Velocity>();
                *e.Get<Velocity>() = std::get<0>(zs::Read<Velocity>(components));
            }
            else if (tag == "name")
            {
                if (!e.Has<xy::Name>())
                    e.Add<xy::Name>();
                *e.Get<xy::Name>() = std::get<0>(zs::Read<xy::Name>(components));
            }
            else
            {
                assert(false);
            }
        }
        return true;
    }
}

namespace ct
{
    asio::awaitable<void> App::Login(asio::io_context& io, const tcp::endpoint& endpoint)
    {
        asio::ip::tcp::socket s{ io };
        co_await s.async_connect(endpoint, asio::use_awaitable);
        auto id = co_await ClientLogin(s, std::chrono::seconds{ 3 });
        if (!id)
            co_return;
        Log("login success " + std::to_string(*id));
        auto agent = std::make_shared<NetAgent>(std::move(s));
        agent->OnError(
            [agent]() {
                Log("net agent on error");
            });

        agent->Listen("entities",
            [agent, this](std::string&& rawWorld)
            {
                zs::StringReader worldArchive{ std::move(rawWorld) };
                curEntities_ = EntityContainer{};
                while (true)
                {
                    auto e = curEntities_.Create();
                    if (!ReadEntity(worldArchive, e))
                    {
                        e.Destroy();
                        break;
                    }
                }
            });

        agent->Listen("voxels",
            [agent, this](std::string&& rawVoxels)
            {
                zs::StringReader in(std::move(rawVoxels));
                while (true)
                {
                    auto rawX = zs::Read<int>(in);
                    if (std::holds_alternative<zs::Error>(rawX))
                        break;
                    auto x = std::get<0>(rawX);
                    auto y = std::get<0>(zs::Read<int>(in));
                    auto z = std::get<0>(zs::Read<int>(in));
                    auto newVoxel = std::get<0>(zs::Read<voxel::Voxel>(in));
                    auto nowVoxel = curVoxels_.Get(x, y, z);
                    *nowVoxel = newVoxel;
                }
            });

        {
            zs::StringWriter out;
            zs::Write(out, std::string("a"));
            agent->Send("set name", out.String());
        }

        co_spawn(co_await asio::this_coro::executor, [agent]() -> asio::awaitable<void>
            {
                co_await agent->ReadRoutine();
            }, asio::detached);

        co_spawn(co_await asio::this_coro::executor, [agent]() -> asio::awaitable<void>
            {
                co_await agent->WriteRoutine();
            }, asio::detached);

        curAgent_ = agent;
        curID_ = *id;

        co_spawn(co_await asio::this_coro::executor, Sync(), asio::detached);
    }

    asio::awaitable<void> App::Sync()
    {
        asio::system_timer t{ io_ };
        while (curAgent_)
        {
            t.expires_after(std::chrono::milliseconds{ 33 });
            co_await t.async_wait(asio::use_awaitable);

            zs::StringWriter out;
            zs::Write(out, Eigen::Vector3f(cameraPos_.data()));
            curAgent_->Send("set position", out.String());
        }
    }

    asio::awaitable<void> App::RefreshServerList(asio::io_context& io)
    {
        cpr::Response r = cpr::Get(cpr::Url{ "http://ct-1302800279.cos.ap-hongkong.myqcloud.com/server.json" });
        if (r.status_code != 200)
            co_return;
        servers_.clear();
        for (auto server : json::parse(r.text))
            servers_.push_back({ server["name"], StringToEndpoint(server["endpoint"]) });
    }

    App::App(const Vector2i& windowSize, GLFWwindow* window)
        :windowSize_(windowSize), window_(window),
        instancedShader_(Shaders::PhongGL::Flag::InstancedTransformation |
            Shaders::PhongGL::Flag::VertexColor)
    {
        instancedMesh_ = MeshTools::compile(Primitives::cubeSolid());
        instancedMesh_.addVertexBufferInstanced(instancedBuffer_,
            1, 0,
            Shaders::PhongGL::TransformationMatrix{},
            Shaders::PhongGL::NormalMatrix{},
            Shaders::PhongGL::Color4{});

        palette_[voxel::Type::Block] = 0xffffff_rgbf;

        co_spawn(io_, RefreshServerList(io_), asio::detached);
    }

    void App::Tick()
    {
        io_.run_for(std::chrono::milliseconds{ 1 });
        TickInput();
        DrawInstanced();
        TickImGui();
        fps_.fire();
    }

    void App::OnMouseMove(float dx, float dy)
    {
        cameraYaw_ -= dx * 0.002f;
        cameraPitch_ -= dy * 0.002f;
    }

    void App::DrawInstanced()
    {
        struct Instance
        {
            Matrix4 transformation;
            Matrix3x3 normal;
            Color4 color;
        };
        std::array<Instance, voxel::Container::size> instances;
        std::atomic_size_t index = 0;

        std::for_each(std::execution::par_unseq,
            std::cbegin(voxel::Container::indices), std::cend(voxel::Container::indices),
            [&](auto voxelIndex)
            {
                auto [x, y, z] = ToTuple(voxelIndex);
                auto voxel = curVoxels_.Get(x, y, z);
                if (!voxel)
                    return;
                if (voxel->type != voxel::Type::Block)
                    return;
                Vector3 pos = { float(x),float(y),float(z) };
                auto transform = Matrix4::translation(pos) *
                    Matrix4::scaling(Vector3{ 0.5f,0.5f,0.5f });
                auto color = palette_[voxel->type];
                instances[index++] = { transform, transform.normalMatrix(), color };
            });
        instancedBuffer_.setData(
            Containers::ArrayView{ instances.data(), index },
            GL::BufferUsage::DynamicDraw);
        instancedMesh_.setInstanceCount(index);

        auto transform = Matrix4::translation(Vector3{ 3.f,3.f,3.f });
        instancedShader_.setLightPositions({ {1.4f, 1.0f, 0.75f, 0.0f} })
            .setDiffuseColor(0xffffff_rgbf)
            .setAmbientColor(Color3::fromHsv(Deg(0.f), 0.f, 0.3f))
            .setProjectionMatrix(projection_)
            .setTransformationMatrix(transform)
            .setNormalMatrix(transform.normalMatrix());
        instancedShader_.draw(instancedMesh_);

        drawVoxels_ = index;
    }

    void App::TickImGui()
    {
        ImGui::Begin("CrazyTang");
        ImGui::Text("fps: %f", fps_.get());
        ImGui::Text("voxel: %d/%llu", drawVoxels_, curVoxels_.x * curVoxels_.y * curVoxels_.z);

        if (curAgent_)
        {
            ImGui::Text("id: %llu", curID_);
        }
        else
        {
            ImGui::Separator();
            ImGui::Text("servers:");
            ImGui::SameLine();
            if (ImGui::Button("refresh"))
                co_spawn(io_, RefreshServerList(io_), asio::detached);

            for (const auto& server : servers_)
                if (ImGui::Button(server.name.c_str()))
                    co_spawn(io_, Login(io_, server.endpoint), asio::detached);

            ImGui::Separator();
        }

        ImGui::DragFloat("camera yaw", &cameraYaw_, 0.01f);
        ImGui::DragFloat("camera pitch", &cameraPitch_, 0.01f);
        ImGui::DragFloat3("camera", cameraPos_.data(), 0.01f);
        {
            auto camera =
                Matrix4::translation(cameraPos_) *
                Matrix4::rotation(Rad{ cameraYaw_ }, Vector3::yAxis()) *
                Matrix4::rotation(Rad{ cameraPitch_ }, Vector3::xAxis());
            camera = camera.inverted();
            projection_ =
                Matrix4::perspectiveProjection(35.0_degf, windowSize_.aspectRatio(), 0.01f, 1000.0f)
                * camera;
        }

        curEntities_.ForEach([](auto e)
            {
                auto id = e.Get<ConnectionID>()->id;
                auto name = "entity " + std::to_string(id);
                if (ImGui::TreeNode(name.c_str()))
                {
                    if (e.Has<Position>())
                    {
                        auto pos = e.Get<Position>()->data;
                        ImGui::Text("pos:%06f %06f %06f", pos.x(), pos.y(), pos.z());
                    }
                    if (e.Has<Velocity>())
                    {
                        auto vel = e.Get<Velocity>()->data;
                        ImGui::Text("vel:%06f %06f %06f", vel.x(), vel.y(), vel.z());
                    }
                    if (e.Has<xy::Name>())
                    {
                        auto name = e.Get<xy::Name>()->data;
                        ImGui::Text("name:%s", name.c_str());
                    }
                    ImGui::TreePop();
                }
            });

        ImGui::End();

        ImGui::Begin("log");
        for (auto beg = logs.rbegin();beg != logs.rend();++beg)
            ImGui::Text(beg->c_str());
        ImGui::End();
    }

    void App::TickInput()
    {
        float speed = 0.1f;

        float forward = 0.f;
        float left = 0.f;
        float up = 0.f;
        if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
            forward += 1.f;
        if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
            forward -= 1.f;
        if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
            left += 1.f;
        if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
            left -= 1.f;
        if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS)
            up -= 1.f;
        if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS)
            up += 1.f;

        auto q = Quaternion::rotation(Rad(cameraYaw_), Vector3::yAxis()) *
            Quaternion::rotation(Rad(cameraPitch_), Vector3::xAxis());

        cameraPos_ += speed * forward * q.transformVector(-Vector3::zAxis());
        cameraPos_ += speed * left * q.transformVector(-Vector3::xAxis());
        cameraPos_ += speed * up * q.transformVector(Vector3::yAxis());
    }
}