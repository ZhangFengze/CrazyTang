#include <memory>
#include "ZSerializer.hpp"
#include "../client_core/Login.h"
#include "../common/Socket.h"
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

asio::awaitable<void> client(asio::io_context& io)
{
    asio::ip::tcp::socket s{ io };
    co_await s.async_connect(ServerEndpoint(), asio::use_awaitable);
    auto pipe = std::make_shared<Pipe<>>(ct::Socket(std::move(s)));
    OnConnected(io, pipe);
}

int main()
{
    asio::io_context io;
    co_spawn(io, client(io), asio::detached);
    io.run();
    return 0;
}