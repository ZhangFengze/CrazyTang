#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <asio.hpp>
#include "../common/Packet.h"
#include "../common/Pipe.h"

namespace ct
{
	inline std::optional<uint64_t> _ToNumber(const std::string& str)
	{
		try
		{
			return std::stoull(str);
		}
		catch (...)
		{
			return std::nullopt;
		}
	}

	inline asio::awaitable<std::optional<uint64_t>>
		ClientLogin(asio::ip::tcp::socket& socket, std::chrono::seconds timeout)
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

		co_await AsyncWritePacket(socket, Packet{ std::string("hello from client") });
		auto read = co_await AsyncReadPacket(socket);
		timeoutHandle.request_stop();

		const std::string_view expected = "hello client, your id is ";
		auto reply = std::string(read.Data(), read.Size());
		if (reply.substr(0, expected.size()) != expected)
			co_return std::nullopt;
		co_return _ToNumber(reply.substr(expected.size()));
	}
}
