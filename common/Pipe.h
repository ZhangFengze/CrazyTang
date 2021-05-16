#pragma once
#include <functional>
#include <system_error>
#include <utility>
#include <memory>
#include "Packet.h"
#include "Socket.h"
#include "asio.hpp"

namespace ct
{
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
