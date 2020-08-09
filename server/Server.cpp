#include "Server.h"
#include "Login.h"
#include "../common/Pipe.h"

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
	Server::Server(asio::io_context &io, const asio::ip::tcp::endpoint &endpoint)
		: io_(io), acceptor_(io, endpoint, std::bind(&Server::OnConnection, this, _1, _2))
	{
	}

	void Server::OnConnection(const std::error_code &error, asio::ip::tcp::socket &&socket)
	{
		if (error)
			return;
		auto pipe = std::make_shared<Pipe<Socket>>(std::move(socket));
		auto login = std::make_shared<Login<Pipe<Socket>>>(pipe, ++connectionID_, io_, std::chrono::seconds{3});
		login->OnSuccess(
			[login, pipe, this]() {
				OnLoginSuccess(pipe, login->id_);
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
		auto info = e.Add<ConnectionInfo>();
		info->connectionID = connectionID;
		info->agent = agent;

		agent->Listen("echo",
					  [agent](std::string &&data) {
						  agent->Send("echo", std::move(data));
					  });

		agent->OnError([e, connectionID, this]() mutable {
			if (e.Valid())
				e.Destroy();
			agents_.erase(connectionID);
		});
	}
} // namespace ct