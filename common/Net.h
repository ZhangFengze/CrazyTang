#pragma once
#include "Packet.h"
#include <asio.hpp>
#include <array>
#include <list>
#include <vector>
#include <functional>
#include <system_error>
#include <utility>
#include <memory>

namespace ct
{
	inline asio::awaitable<void> AsyncWritePacket(asio::ip::tcp::socket& socket, Packet&& packet)
	{
		uint32_t lengthBuffer[1]{ packet.Size() };
		std::array<asio::const_buffer, 2> buffers
		{
			asio::buffer(lengthBuffer),
			asio::buffer((const void*)packet.Data(),packet.Size())
		};
		if (4 + packet.Size() != co_await asio::async_write(socket, buffers, asio::use_awaitable))
			throw asio::error::make_error_code(asio::error::broken_pipe);
	}

	inline asio::awaitable<Packet> AsyncReadPacket(asio::ip::tcp::socket& socket)
	{
		uint32_t lengthBuffer[1];
		if (4 != co_await asio::async_read(socket, asio::buffer(lengthBuffer), asio::use_awaitable))
			throw asio::error::make_error_code(asio::error::eof);
		uint32_t length = lengthBuffer[0];

		std::array<char, 1024 * 1024> buffer;
		if (length > buffer.size())
			throw asio::error::make_error_code(asio::error::no_buffer_space);

		if (length != co_await asio::async_read(socket, asio::buffer(buffer, length), asio::use_awaitable))
			throw asio::error::make_error_code(asio::error::eof);
		co_return Packet{ buffer.data(), length };
	}
    
	class CoroPipe
	{
	public:
		CoroPipe(asio::ip::tcp::socket socket)
			:socket_(std::move(socket)) {}

		void Go()
		{
			co_spawn(socket_.get_executor(), Read(), asio::detached);
			co_spawn(socket_.get_executor(), Write(), asio::detached);
		}

		void OnPacket(std::function<void(Packet&&)> handler) { packetHandler_ = handler; }

		void SendPacket(Packet&& packet)
		{
			if (broken_)
				return;
			out_.emplace_back(std::move(packet));
		}
		void SendPacket(const Packet& packet) { return SendPacket(Packet(packet)); }

		void OnBroken(std::function<void(void)> handler) { brokenHandler_ = handler; }
		bool IsBroken() const { return broken_; }

	private:
		asio::awaitable<void> Read()
		{
			while (!broken_)
			{
				auto packet = co_await AsyncReadPacket(socket_);
				packetHandler_(std::move(packet));
			}
		}

		asio::awaitable<void> Write()
		{
			while (!broken_)
			{
				// TODO optimize
				if (out_.empty())
				{
					asio::steady_timer t{ co_await asio::this_coro::executor };
					t.expires_after(std::chrono::milliseconds{ 0 });
					co_await t.async_wait(asio::use_awaitable);
					continue;
				}

				auto packet = out_.front();
				out_.pop_front();
				co_await AsyncWritePacket(socket_, std::move(packet));
			}
		}

	private:
		asio::ip::tcp::socket socket_;

		std::function<void(Packet&&)> packetHandler_;

		std::list<Packet> out_;

		std::function<void()> brokenHandler_;
		bool broken_ = false;// TODO
	};
}