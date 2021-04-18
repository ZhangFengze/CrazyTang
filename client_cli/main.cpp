#include <memory>
#include "ZSerializer.hpp"
#include "../client_core/Login.h"
#include "../common/Pipe.h"
#include "../common/NetAgent.h"
#include "../common/AsyncConnect.h"
#include "../common/Entity.h"
#include "../common/Player.h"
#include "../common/Voxel.h"
#include "../common/Math.h"

using namespace ct;
using asio::ip::tcp;

namespace
{
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

    void OnLoginSuccess(asio::io_context& io, std::shared_ptr<Pipe<>> pipe)
    {
        auto agent = std::make_shared<NetAgent<>>(pipe);
        agent->OnError(
            [agent]() {
                printf("net agent on error\n");
            });

        agent->Listen("echo",
            [agent](std::string&& data) {
                printf("net agent on echo:%s\n", data.c_str());
            });
        agent->Send("echo", "hello?");

        agent->Listen("broadcast",
            [agent](std::string&& data) {
                zs::StringReader in( std::move(data) );
                auto from = zs::Read<uint64_t>(in);
                auto content = zs::Read<std::string>(in);
                printf("net agent on broadcast, from %llu, content:%s\n", std::get<0>(from), std::get<0>(content).c_str());
            });
        agent->Send("broadcast", "hello everyone?");

        agent->Listen("list online",
            [agent](std::string&& data) {
                zs::StringReader in( std::move(data) );
                auto size = zs::Read<size_t>(in);
                printf("net agent on list online: %llu online\n", std::get<0>(size));
                for (size_t i = 0; i < std::get<0>(size); ++i)
                {
                    auto id = zs::Read<uint64_t>(in);
                    printf("%llu\n", std::get<0>(id));
                }
            });
        agent->Send("list online", "");

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

        agent->Listen("world",
            [](std::string&& rawWorld)
            {
                printf("net agent on world,");
                zs::StringReader worldIn{ std::move(rawWorld) };
                while (true)
                {
                    auto type = zs::Read<std::string>(worldIn);
                    if (std::holds_alternative<zs::Error>(type)) break;

                    if (std::get<0>(type) == "player")
                    {
                        auto id = zs::Read<uint64_t>(worldIn);
                        zs::StringReader entityArchive{ std::get<0>(zs::Read<std::string>(worldIn)) };
                        EntityContainer entities;
                        auto e = entities.Create();
                        LoadPlayer(entityArchive, e);
                        printf("player [id:%llu, position:%f %f %f, velocity:%f %f %f],", std::get<0>(id),
                            e.Get<Position>()->data.x(),
                            e.Get<Position>()->data.y(),
                            e.Get<Position>()->data.z(),
                            e.Get<Velocity>()->data.x(),
                            e.Get<Velocity>()->data.y(),
                            e.Get<Velocity>()->data.z());
                    }
                    else if (std::get<0>(type) == "voxel")
                    {
                        auto voxel = std::get<0>(zs::Read<Voxel>(worldIn));
                        printf("voxel [index:(%d,%d), type:%d],", voxel.index.x(), voxel.index.y(), voxel.type);
                    }
                    printf("\n");
                }
                printf("-------\n");
            });
    }

    void OnConnected(asio::io_context& io, std::shared_ptr<Pipe<>> pipe)
    {
        auto login = std::make_shared<Login<>>(pipe, io, std::chrono::seconds{ 3 });
        login->OnSuccess(
            [login, &io, pipe](uint64_t id) {
                printf("login success %llu\n", id);
                OnLoginSuccess(io, pipe);
            });
        login->OnError(
            [login]() {
                printf("login error\n");
            });
    }
} // namespace

int main()
{
    asio::io_context io;
    AsyncConnect(io, ServerEndpoint(),
        [&io](const std::error_code& error, std::shared_ptr<Socket> socket) {
            if (error)
            {
                printf("async connect error\n");
                return;
            }

            auto pipe = std::make_shared<Pipe<>>(std::move(*socket));
            OnConnected(io, pipe);
        });
    io.run();
    return 0;
}