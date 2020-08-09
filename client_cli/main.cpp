#include <memory>
#include "../client_logic/Login.h"
#include "../common/Pipe.h"
#include "../common/NetAgent.h"
#include "../common/AsyncConnect.h"
#include "../common/Serializer.h"

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
        agent->OnError(
            [agent]() {
                printf("net agent on error\n");
            });

        agent->Listen("echo",
                      [agent](std::string &&data) {
                          printf("net agent on echo:%s\n", data.c_str());
                      });
        agent->Send("echo", "hello?");

        agent->Listen("broadcast",
                      [agent](std::string &&data) {
                          InputStringArchive in{std::move(data)};
                          uint64_t from;
                          in.Read(from);
                          std::string content;
                          in.Read(content);
                          printf("net agent on broadcast, from %llu, content:%s\n", from, content.c_str());
                      });
        agent->Send("broadcast", "hello everyone?");

        agent->Listen("list online",
                      [agent](std::string &&data) {
                          InputStringArchive in{std::move(data)};
                          size_t size;
                          in.Read(size);
                          printf("net agent on list online: %llu online\n", size);
                          for (size_t i = 0; i < size; ++i)
                          {
                              uint64_t id;
                              in.Read(id);
                              printf("%llu\n", id);
                          }
                      });
        agent->Send("list online", "");
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