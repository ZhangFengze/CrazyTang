#include <memory>
#include "../client_logic/Login.h"
#include "../common/Pipe.h"
#include "../common/NetAgent.h"
#include "../common/AsyncConnect.h"

using namespace ct;
using asio::ip::tcp;

namespace
{
    tcp::endpoint StringToEndpoint(const std::string &str)
    {
        auto pos = str.find(':');
        if (pos == std::string::npos)
            return {};
        auto ip = str.substr(0, pos);
        int port = std::stoi(str.substr(pos + 1));
        return {asio::ip::make_address_v4(ip), (unsigned short)port};
    }

    tcp::endpoint ServerEndpoint()
    {
        return StringToEndpoint("127.0.0.1:3377");
    }

    void OnLoginSuccess(asio::io_context &io, std::shared_ptr<Pipe<>> pipe)
    {
        auto agent = std::make_shared<NetAgent<>>(pipe);
        agent->Listen("test",
                      [agent](std::string &&data) {
                          printf("net agent on data %s\n", data.c_str());
                      });
        agent->OnError(
            [agent]() {
                printf("net agent on error\n");
            });

        agent->Send("test", "hello?");
    }

    void OnConnected(asio::io_context &io, std::shared_ptr<Pipe<>> pipe)
    {
        auto login = std::make_shared<Login<>>(pipe, io, std::chrono::seconds{3});
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
                 [&io](const std::error_code &error, std::shared_ptr<Socket> socket) {
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