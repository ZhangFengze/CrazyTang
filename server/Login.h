#pragma once
#include <cstdint>
#include <memory>
#include <chrono>
#include <stop_token>
#include <asio.hpp>
#include "../common/Packet.h"
#include "../common/Socket.h"

namespace ct
{
	inline asio::awaitable<bool> ServerLogin(asio::ip::tcp::socket& socket, uint64_t id, std::chrono::seconds timeout)
	{
		std::stop_source timeoutHandle;
		co_spawn(co_await asio::this_coro::executor,
			[&socket, timeout, stop = timeoutHandle.get_token()]()->asio::awaitable<void>
		{
			asio::steady_timer timer{ co_await asio::this_coro::executor };
			timer.expires_after(timeout);

			co_await timer.async_wait(asio::use_awaitable);
			if (!stop.stop_requested())
				socket.close();
		}, asio::detached);

		auto read = co_await AsyncReadPacket(socket);
		timeoutHandle.request_stop();
		if (read != std::string("hello from client"))
			co_return false;

		auto reply = "hello client, your id is " + std::to_string(id);
		co_await AsyncWritePacket(socket, Packet{ reply });
		co_return true;
	}
}
