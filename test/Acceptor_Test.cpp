#include <catch.hpp>
#include "../common/Acceptor.h"

using asio::ip::tcp;

namespace
{
	tcp::endpoint AvailableLocalEndpoint()
	{
		return tcp::endpoint{ asio::ip::make_address("127.0.0.1"),53453 };
	}
}

TEST_CASE("accpetor callback", "[Acceptor]") 
{
	asio::io_context io;
	asio::error_code error;
	auto endpoint = AvailableLocalEndpoint();
	int called = 0;

	ct::Acceptor acceptor{ io,endpoint,
		[&called](const asio::error_code& error, tcp::socket&& socket)
	{
		++called;
	} };

	{
		tcp::socket socket{ io };
		socket.connect(endpoint, error);
		io.poll();
	}
	{
		tcp::socket socket{ io };
		socket.connect(endpoint, error);
		io.poll();
	}
	REQUIRE(called == 2);
}

TEST_CASE("accpetor should callback once with error, after destruction", "[Acceptor]") 
{
	asio::io_context io;
	auto endpoint = AvailableLocalEndpoint();
	bool hasError = false;
	bool called = false;

	{
		ct::Acceptor acceptor{ io,endpoint,
			[&hasError,&called](const asio::error_code& error, tcp::socket&& socket)
		{
			hasError = error.operator bool();
			called = true;
		} };
	}
	io.poll();
	REQUIRE(hasError);
	REQUIRE(called);
}