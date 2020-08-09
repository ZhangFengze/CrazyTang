#include "Acceptor.h"

namespace ct
{
	Acceptor::Acceptor(asio::io_context& io, asio::ip::tcp::endpoint endpoint, std::function<void(const asio::error_code&, asio::ip::tcp::socket&&)> callback)
		:acceptor_(io, endpoint)
	{
		StartAccept(callback);
	}

	void Acceptor::StartAccept(std::function<void(const asio::error_code&, asio::ip::tcp::socket&&)> callback)
	{
		std::weak_ptr<bool> alive = alive_;
		acceptor_.async_accept([this, alive, callback](const asio::error_code& error, asio::ip::tcp::socket&& socket)
		{
			callback(error, std::move(socket));
			if (!alive.expired())
				this->StartAccept(callback);
		});
	}
}