#include "Server.h"
#include "Login.h"
#include "../../common/source/Pipe.h"

using namespace std::placeholders;

namespace ct
{
	Server::Server(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint)
		:io_(io), acceptor_(io, endpoint, std::bind(&Server::OnConnection, this, _1, _2))
	{
	}

	void Server::OnConnection(const std::error_code& error, asio::ip::tcp::socket&& socket)
	{
		if (error)
			return;
		auto pipe = std::make_shared<Pipe<Socket>>(std::move(socket));
		auto login = std::make_shared<Login<Pipe<Socket>>>(pipe, ++connectionID_, io_, std::chrono::seconds{ 3 });
		login->OnSuccess(
			[login]()
		{
			login->OnSuccess(nullptr);
		});
		login->OnError(
			[login]()
		{
			login->OnError(nullptr);
		});
	}
}