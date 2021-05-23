#include "client_gui.h"
#include "imgui.h"
#include "ZSerializer.hpp"
#include "../client_core/Login.h"
#include "../common/Position.h"
#include "../common/Velocity.h"
#include "../common/Voxel.h"
#include "../common/Math.h"
#include "../common/UUID.h"

using namespace ct;
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
    {
        using namespace Math::Literals;

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

        _transformation =
            Matrix4::rotationX(30.0_degf) * Matrix4::rotationY(40.0_degf);
        _projection =
            Matrix4::perspectiveProjection(
                35.0_degf, Vector2{windowSize}.aspectRatio(), 0.01f, 100.0f) *
            Matrix4::translation(Vector3::zAxis(-10.0f));
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
        _shader.setLightPositions({ {1.4f, 1.0f, 0.75f, 0.0f} })
            .setDiffuseColor(_color)
            .setAmbientColor(Color3::fromHsv({ _color.hue(), 1.0f, 0.3f }))
            .setTransformationMatrix(_transformation)
            .setNormalMatrix(_transformation.normalMatrix())
            .setProjectionMatrix(_projection)
            .draw(_mesh);
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