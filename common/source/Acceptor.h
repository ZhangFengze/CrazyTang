#pragma once
#include <asio.hpp>

namespace ct
{
	class Acceptor
	{
	public:
		Acceptor(asio::io_context& io, asio::ip::tcp::endpoint endpoint,
			std::function<void(const asio::error_code&, asio::ip::tcp::socket&&)> callback);

	private:
		void StartAccept(std::function<void(const asio::error_code&, asio::ip::tcp::socket&&) > callback);

	private:
		std::shared_ptr<bool> alive_ = std::make_shared<bool>();
		asio::ip::tcp::acceptor acceptor_;
	};
}
