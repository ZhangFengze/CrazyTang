#include <memory>
#include "ZSerializer.hpp"
#include "../client_core/Login.h"
#include "../common/Pipe.h"
#include "../common/NetAgent.h"
#include "../common/AsyncConnect.h"
#include "../common/Entity.h"
#include "../common/Position.h"
#include "../common/Velocity.h"
#include "../common/Voxel.h"
#include "../common/Math.h"
#include "../common/UUID.h"

using namespace ct;
using asio::ip::tcp;

namespace
{
    struct ConnectionID
    {
        uint64_t id;
    };

    template<typename In>
    bool ReadEntity(In& in, ct::EntityHandle e)
    {
        auto uuid = zs::Read<UUID>(in);
        if (std::holds_alternative<zs::Error>(uuid))
            return false;
        printf("uuid:%llu, ", std::get<0>(uuid).id);

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
                printf("id:%llu, ", e.Get<ConnectionID>()->id);
            }
            else if (tag == "position")
            {
                if (!e.Has<Position>())
                    e.Add<Position>();
                *e.Get<Position>() = std::get<0>(zs::Read<Position>(components));
                printf("position:%f %f %f, ",
                    e.Get<Position>()->data.x(),
                    e.Get<Position>()->data.y(),
                    e.Get<Position>()->data.z());
            }
            else if (tag == "velocity")
            {
                if (!e.Has<Velocity>())
                    e.Add<Velocity>();
                *e.Get<Velocity>() = std::get<0>(zs::Read<Velocity>(components));
                printf("velocity:%f %f %f, ",
                    e.Get<Velocity>()->data.x(),
                    e.Get<Velocity>()->data.y(),
                    e.Get<Velocity>()->data.z());
            }
            else if (tag == "voxel")
            {
                if (!e.Has<Voxel>())
                    e.Add<Voxel>();
                auto voxel = e.Get<Voxel>();
                *voxel = std::get<0>(zs::Read<Voxel>(components));
                printf("voxel index:(%d,%d), voxel type:%d, ",
                    voxel->index.x(),
                    voxel->index.y(),
                    voxel->type);
            }
            else
            {
                assert(false);
            }
            printf("\n");
        }
        printf("-------\n");
        return true;
    }

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

                EntityContainer entities;
                while(true)
                {
                    auto e = entities.Create();
                    if(!ReadEntity(worldIn, e))
                        break;
                }
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