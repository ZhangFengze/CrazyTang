#include "Server.h"

int main()
{
	asio::io_context io;
	auto endpoint = asio::ip::tcp::endpoint{ asio::ip::make_address("127.0.0.1"),3377 };
	ct::Server server{ io,endpoint };
	io.run();
	return 0;
}