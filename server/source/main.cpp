#ifndef CT_TEST
#include "Server.h"

int main()
{
	asio::io_context io;
	auto endpoint = asio::ip::tcp::endpoint{ asio::ip::make_address("127.0.0.1"),3377 };
	ct::BroadcastServer server{ io,endpoint };
	io.run();
	return 0;
}

#else

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

int main(int argc, char* argv[])
{
	return Catch::Session().run(argc, argv);
}

#endif