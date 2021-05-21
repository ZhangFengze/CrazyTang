#include <memory>
#include "imgui.h"
#include "ZSerializer.hpp"
#include "../client_core/Login.h"
#include "../common/Net.h"
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
    asio::io_context io;

    std::shared_ptr<ct::NetAgent> curAgent;
    uint64_t curID = 0;

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

    asio::awaitable<void> client(asio::io_context& io)
    {
        asio::ip::tcp::socket s{ io };
        co_await s.async_connect(ServerEndpoint(), asio::use_awaitable);
        auto id = co_await ClientLogin(s, std::chrono::seconds{ 3 });
        if (!id)
            co_return;
        Log("login success "+std::to_string(*id));
        auto agent = std::make_shared<NetAgent>(std::move(s));
        agent->OnError(
            [agent]() {
                Log("net agent on error");
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
} // namespace

namespace zs
{
    void client_gui_init()
    {
    }

    void client_gui_tick()
    {
        io.run_for(std::chrono::milliseconds{ 1 });

        ImGui::Begin("CrazyTang");

        if (curAgent)
        {
            ImGui::Text("id: %llu", curID);
        }
        else
        {
            if (ImGui::Button("login"))
                co_spawn(io, client(io), asio::detached);
        }

        for(auto beg=logs.rbegin();beg!=logs.rend();++beg)
            ImGui::Text(beg->c_str());

        ImGui::End();
    }
}