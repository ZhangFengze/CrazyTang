#include "Server.h"
#include "Login.h"
#include "Player.h"
#include "../common/Pipe.h"
#include "../common/MoveSystem.h"
#include <thread>

using namespace std::placeholders;

namespace
{
	struct ConnectionInfo
	{
		uint64_t connectionID;
		std::weak_ptr<ct::NetAgent<>> agent;
	};
} // namespace

namespace ct
{
	void Server::Run()
	{
		auto endpoint = asio::ip::tcp::endpoint{ asio::ip::make_address("127.0.0.1"),3377 };
		ct::Acceptor acceptor{io_,endpoint,std::bind(&Server::OnConnection, this, _1, _2)};

		while (true)
		{
			io_.poll();
			move_system::Process(entities_, 0.1f);
			std::this_thread::sleep_for(std::chrono::microseconds{1});
		}
	}

	void Server::OnConnection(const std::error_code &error, asio::ip::tcp::socket &&socket)
	{
		if (error)
			return;
		auto pipe = std::make_shared<Pipe<Socket>>(std::move(socket));
		auto login = std::make_shared<Login<Pipe<Socket>>>(pipe, ++connectionID_, io_, std::chrono::seconds{3});
		login->OnSuccess(
			[login, pipe, this](uint64_t id) {
				OnLoginSuccess(pipe, id);
			});
		login->OnError(
			[login]() {
			});
	}

	void Server::OnLoginSuccess(std::shared_ptr<Pipe<>> pipe, uint64_t connectionID)
	{
		auto agent = std::make_shared<NetAgent<>>(pipe);
		agents_[connectionID] = agent;

		auto e = entities_.Create();

		InitPlayer(e);

		auto info = e.Add<ConnectionInfo>();
		info->connectionID = connectionID;
		info->agent = agent;

		agent->Listen("echo",
					  [agent](std::string &&data) {
						  agent->Send("echo", std::move(data));
					  });

		agent->Listen("broadcast",
					  [this, connectionID](std::string &&data) {
						  OutputStringArchive out;
						  out.Write(connectionID);
						  out.Write(std::move(data));
						  auto reply = out.String();
						  for (auto &[_, other] : agents_)
							  other->Send("broadcast", reply);
					  });

		agent->Listen("list online",
					  [this, agent](std::string &&data) {
						  OutputStringArchive out;
						  out.Write(agents_.size());
						  for (auto &[id, _] : agents_)
							  out.Write(id);
						  agent->Send("list online", out.String());
					  });

		agent->OnError([e, connectionID, this]() mutable {
			if (e.Valid())
				e.Destroy();
			agents_.erase(connectionID);
		});
	}
} // namespace ct