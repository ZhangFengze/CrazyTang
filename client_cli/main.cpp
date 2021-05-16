#include <memory>
#include "ZSerializer.hpp"
#include "../client_core/Login.h"
#include "../common/Net.h"
#include "../common/NetAgent.h"
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
} // namespace

asio::awaitable<void> client(asio::io_context& io)
{
    asio::ip::tcp::socket s{ io };
    co_await s.async_connect(ServerEndpoint(), asio::use_awaitable);
    auto id = co_await ClientLogin(s, std::chrono::seconds{ 3 });
    if (!id)
        co_return;
    printf("login success %llu\n", *id);
    auto pipe = std::make_shared<Pipe>(std::move(s));
    pipe->Go();
    auto agent = std::make_shared<NetAgent<Pipe>>(pipe);
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

int main()
{
    asio::io_context io;
    co_spawn(io, client(io), asio::detached);
    io.run();
    return 0;
}