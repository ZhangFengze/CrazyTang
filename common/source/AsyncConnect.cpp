#include "AsyncConnect.h"

namespace ct
{
	void AsyncConnect(asio::io_context& io, const asio::ip::tcp::endpoint& endpoint,
		std::function<void(const std::error_code&, std::shared_ptr<Socket>)> callback)
	{
		auto socket = std::make_shared<asio::ip::tcp::socket>(io);
		socket->async_connect(endpoint,
			[socket, callback](const std::error_code& error)
		{
			if (error)
				return callback(error, nullptr);
			callback(std::error_code{}, std::make_shared<Socket>(std::move(*socket)));
		});
	}
}
