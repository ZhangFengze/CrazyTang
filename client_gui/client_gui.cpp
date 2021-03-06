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
#include <Magnum/Primitives/Icosphere.h>
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

#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}
namespace
{
    using namespace physx;

    PxDefaultAllocator		gAllocator;
    PxDefaultErrorCallback	gErrorCallback;

    PxFoundation* gFoundation = NULL;
    PxPhysics* gPhysics = NULL;

    PxDefaultCpuDispatcher* gDispatcher = NULL;
    PxScene* gScene = NULL;

    PxMaterial* gMaterial = NULL;

    PxRigidStatic* CreateBoxStatic(const PxTransform& t, const PxVec3& halfExtents)
    {
        PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtents), *gMaterial);
        auto body = gPhysics->createRigidStatic(t);
        body->attachShape(*shape);
        gScene->addActor(*body);
        shape->release();
        return body;
    }

    void initPhysics()
    {
        gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
        gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);
        gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

        PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        gDispatcher = PxDefaultCpuDispatcherCreate(2);
        sceneDesc.cpuDispatcher = gDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        gScene = gPhysics->createScene(sceneDesc);
    }

    void stepPhysics()
    {
        gScene->simulate(1.0f / 60.0f);
        gScene->fetchResults(true);
    }

    void cleanupPhysics(bool /*interactive*/)
    {
        PX_RELEASE(gScene);
        PX_RELEASE(gDispatcher);
        PX_RELEASE(gPhysics);
        PX_RELEASE(gFoundation);
    }
}
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
        voxelShader_(Shaders::PhongGL::Flag::InstancedTransformation |
            Shaders::PhongGL::Flag::VertexColor)
    {
        voxelMesh_ = MeshTools::compile(Primitives::cubeSolid());
        voxelMesh_.addVertexBufferInstanced(voxelBuffer_,
            1, 0,
            Shaders::PhongGL::TransformationMatrix{},
            Shaders::PhongGL::NormalMatrix{},
            Shaders::PhongGL::Color4{});

        palette_[voxel::Type::Block] = 0xffffff_rgbf;

        playerMesh_ = MeshTools::compile(Primitives::icosphereSolid(2));

        co_spawn(io_, RefreshServerList(io_), asio::detached);

        initPhysics();
    }

    void App::Tick()
    {
        io_.run_for(std::chrono::milliseconds{ 1 });
        TickInput();
        TickVoxelsView();
        stepPhysics();
        DrawVoxels();
        DrawPlayers();
        TickImGui();
        fps_.fire();
    }

    void App::OnMouseMove(float dx, float dy)
    {
        cameraYaw_ -= dx * 0.002f;
        cameraPitch_ -= dy * 0.002f;
    }

    void App::DrawVoxels()
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
        voxelBuffer_.setData(
            Containers::ArrayView{ instances.data(), index },
            GL::BufferUsage::DynamicDraw);
        voxelMesh_.setInstanceCount(index);

        voxelShader_.setLightPositions({ {1.4f, 1.0f, 0.75f, 0.0f} })
            .setDiffuseColor(0xffffff_rgbf)
            .setAmbientColor(Color3::fromHsv(Deg(0.f), 0.f, 0.3f))
            .setProjectionMatrix(projection_);
        voxelShader_.draw(voxelMesh_);

        drawVoxels_ = index;
    }

    void App::DrawPlayers()
    {
        curEntities_.ForEach([&](auto e)
            {
                if (!e.Has<Position>())
                    return;
                auto pos = e.Get<Position>()->data;
                auto transform = Matrix4::translation(Vector3{ pos.x(), pos.y(), pos.z() }) *
                    Matrix4::scaling(Vector3{ 0.5f,0.5f,0.5f });

                playerShader_.setLightPositions({ {1.4f, 1.0f, 0.75f, 0.0f} })
                    .setDiffuseColor(0x0000ff_rgbf)
                    .setAmbientColor(Color3::fromHsv(Deg(0.f), 0.f, 0.3f))
                    .setProjectionMatrix(projection_)
                    .setTransformationMatrix(transform)
                    .setNormalMatrix(transform.normalMatrix());
                playerShader_.draw(playerMesh_);
            });
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

    void App::TickVoxelsView()
    {
        for (auto index : voxel::Container::indices)
        {
            auto data = curVoxels_.GetNoCheck(index.x(), index.y(), index.z());
            auto view = curVoxelsView_.GetNoCheck(index.x(), index.y(), index.z());
            if (data->type == voxel::Type::Block)
            {
                if (!view->rigid)
                {
                    view->rigid = CreateBoxStatic(PxTransform{ PxVec3(index.x(),index.y(),index.z()) },
                        PxVec3{ 0.5f,0.5f,0.5f });
                }
            }
            else
            {
                if (view->rigid)
                {
                    view->rigid->release();
                    view->rigid = nullptr;
                }
            }
        }
    }
}