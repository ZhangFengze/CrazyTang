#ifdef CT_TEST
#include <catch.hpp>
#include "Server.h"

using asio::ip::tcp;

namespace
{
	tcp::endpoint AvailableLocalEndpoint()
	{
		return tcp::endpoint{ asio::ip::make_address("127.0.0.1"),53453 };
	}
}

TEST_CASE("broadcast", "[broadcast server]")
{
	asio::io_context io;
	auto endpoint = AvailableLocalEndpoint();
	ct::BroadcastServer server{ io,endpoint };

	struct TestClient
	{
		TestClient(std::shared_ptr<ct::Connection> socket, int index)
			:socket_(socket), index_(index)
		{
		}

		int NextExpectNumber()
		{
			return received_ >= index_ ? received_ + 1 : received_;
		}

		void CheckRead()
		{
			auto expected = "hello from " + std::to_string(NextExpectNumber());
			socket_->AsyncReadPacket([=](std::error_code error, const char* data, size_t size)
			{
				REQUIRE(!error);
				auto got = std::string{ data,size - 1 };
				REQUIRE(got == expected);

				received_++;
				if (received_ < 4)
					CheckRead();
			});
		}

		std::shared_ptr<ct::Connection> socket_;
		int index_;
		int received_ = 0;
	};

	std::vector<TestClient> clients;
	for (int i = 0; i < 5; ++i)
	{
		tcp::socket socket{ io };
		socket.connect(endpoint);
		clients.emplace_back(
			std::make_shared<ct::Connection>(std::move(socket)), i);
	}
	io.poll();

	for (int index = 0; index < 5; ++index)
	{
		std::string msg = "hello from " + std::to_string(index);
		clients[index].socket_->AsyncWritePacket(msg.data(), msg.size() + 1,
			[](std::error_code error)
		{
			REQUIRE(!error);
		});
	}
	io.poll();

	for (int index = 0; index < 5; ++index)
		clients[index].CheckRead();
	
	io.poll();
}

#endif