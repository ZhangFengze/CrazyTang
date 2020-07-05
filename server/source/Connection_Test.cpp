#ifdef CT_TEST
#include <catch.hpp>
#include "Connection.h"

using asio::ip::tcp;

namespace
{
	tcp::endpoint AvailableLocalEndpoint()
	{
		return tcp::endpoint{ asio::ip::make_address("127.0.0.1"),53453 };
	}
}

TEST_CASE("construction", "[Connection]") 
{
	asio::io_context io;
	auto endpoint = AvailableLocalEndpoint();

	tcp::acceptor acceptor{ io,};
	acceptor.async_accept(endpoint, 
		[](const asio::error_code&, tcp::socket&& socket)
	{
		ct::Connection connection{ std::move(socket) };
	});

	tcp::socket socket{ io };
	socket.async_connect(endpoint, [](asio::error_code){});

	io.run();
}
#endif
