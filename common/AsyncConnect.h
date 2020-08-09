#pragma once
#include <asio.hpp>
#include "Socket.h"

namespace ct
{
	void AsyncConnect(asio::io_context&, const asio::ip::tcp::endpoint&,
		std::function<void(const std::error_code&, std::shared_ptr<Socket>)>);

}
