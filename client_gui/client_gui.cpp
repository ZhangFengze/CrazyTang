#include "client_gui.h"
#include "imgui.h"
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

    tcp::endpoint ServerEndpoint()
    {
        return StringToEndpoint("127.0.0.1:33773");
    }

    template<typename In>
    bool ReadEntity(In& in, ct::EntityHandle e)
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

namespace zs
{
    asio::awaitable<void> App::Login(asio::io_context& io)
    {
        asio::ip::tcp::socket s{ io };
        co_await s.async_connect(ServerEndpoint(), asio::use_awaitable);
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
                curEntities = ct::EntityContainer{};
                while (true)
                {
                    auto e = curEntities.Create();
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
                    auto newVoxel = std::get<0>(zs::Read<ct::voxel::Voxel>(in));
                    auto nowVoxel = curVoxels.Get(x, y, z);
                    *nowVoxel = newVoxel;
                }
            });

        {
            zs::StringWriter out;
            zs::Write(out, Eigen::Vector3f{ 1.f,0,0 });
            agent->Send("set position", out.String());
        }

        {
            zs::StringWriter out;
            zs::Write(out, Eigen::Vector3f{ 0,1.f,0 });
            agent->Send("set velocity", out.String());
        }

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

        curAgent = agent;
        curID = *id;
    }

    App::App(const Vector2i& windowSize)
        :windowSize_(windowSize)
    {
        Trade::MeshData cube = Primitives::cubeSolid();

        GL::Buffer vertices;
        vertices.setData(MeshTools::interleave(cube.positions3DAsArray(),
            cube.normalsAsArray()));

        std::pair<Containers::Array<char>, MeshIndexType> compressed =
            MeshTools::compressIndices(cube.indicesAsArray());
        GL::Buffer indices;
        indices.setData(compressed.first);

        _mesh.setPrimitive(cube.primitive())
            .setCount(cube.indexCount())
            .addVertexBuffer(std::move(vertices), 0, Shaders::PhongGL::Position{},
                Shaders::PhongGL::Normal{})
            .setIndexBuffer(std::move(indices), 0, compressed.second);
        _color = Color3::fromHsv({ 35.0_degf, 1.0f, 1.0f });
    }

    void App::Tick()
    {
        io.run_for(std::chrono::milliseconds{ 1 });
        Draw();
        TickImGui();
    }

    void App::Draw()
    {
        curEntities.ForEach([&](auto e)
            {
                auto pos = e.Get<Position>()->data;
                auto transform = Matrix4::translation(Vector3{ pos.x(),pos.y(),pos.z() });
                _shader.setLightPositions({ {1.4f, 1.0f, 0.75f, 0.0f} })
                    .setDiffuseColor(_color)
                    .setAmbientColor(Color3::fromHsv({ _color.hue(), 1.0f, 0.3f }))
                    .setTransformationMatrix(transform)
                    .setNormalMatrix(transform.normalMatrix())
                    .setProjectionMatrix(_projection)
                    .draw(_mesh);
            });

        for (size_t x = 0;x < curVoxels.x;++x)
        {
            for (size_t y = 0;y < curVoxels.y;++y)
            {
                for (size_t z = 0;z < curVoxels.z;++z)
                {
                    auto type = curVoxels.Get(x, y, z)->type;
                    Vector3 pos{ float(x),float(y),float(z) };
                    auto transform = Matrix4::translation(pos);
                    if (type == ct::voxel::Type::Block)
                    {
                        _shader.setLightPositions({ {1.4f, 1.0f, 0.75f, 0.0f} })
                            .setDiffuseColor(_color)
                            .setAmbientColor(Color3::fromHsv({ _color.hue(), 1.0f, 0.3f }))
                            .setTransformationMatrix(transform)
                            .setNormalMatrix(transform.normalMatrix())
                            .setProjectionMatrix(_projection)
                            .draw(_mesh);
                    }
                }
            }
        }
    }

    void App::TickImGui()
    {
        ImGui::Begin("CrazyTang");

        if (curAgent)
        {
            ImGui::Text("id: %llu", curID);
        }
        else
        {
            if (ImGui::Button("login"))
                co_spawn(io, Login(io), asio::detached);
        }

        ImGui::DragFloat3("box", boxPos.data(), 0.01f);
        _transformation = Matrix4::translation(boxPos);

        ImGui::DragFloat("camera yaw", &cameraYaw, 0.01f);
        ImGui::DragFloat("camera pitch", &cameraPitch, 0.01f);
        ImGui::DragFloat3("camera", cameraPos.data(), 0.01f);
        {
            auto camera =
                Matrix4::translation(cameraPos) *
                Matrix4::rotation(Rad{ cameraYaw }, Vector3::yAxis()) *
                Matrix4::rotation(Rad{ cameraPitch }, Vector3::xAxis());
            camera = camera.inverted();
            _projection =
                Matrix4::perspectiveProjection(35.0_degf, windowSize_.aspectRatio(), 0.01f, 100.0f)
                * camera;
        }

        curEntities.ForEach([](auto e)
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
                        auto vel = e.Get<xy::Name>()->data;
                        ImGui::Text("name:%s", vel);
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
}