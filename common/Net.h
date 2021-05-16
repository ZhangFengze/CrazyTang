#pragma once
#include "Packet.h"
#include <ZSerializer.hpp>
#include <asio.hpp>
#include <array>
#include <list>
#include <vector>
#include <functional>
#include <system_error>
#include <utility>
#include <memory>
#include <string>
#include <cassert>

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

	class Pipe
	{
	public:
		Pipe(asio::ip::tcp::socket socket)
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

	class NetAgent
	{
	public:
		NetAgent(asio::ip::tcp::socket socket)
			:socket_(std::move(socket)) {}

		void Listen(const std::string& tag, std::function<void(std::string&&)> handler)
		{
			assert(listeners_.find(tag) == listeners_.end());
			listeners_[tag] = handler;
		}

		void OnError(std::function<void()> handler)
		{
			errorHandler_ = handler;
		}

		void Send(const std::string& tag, const std::string& content)
		{
			zs::StringWriter out;
			zs::Write(out, tag);
			zs::Write(out, content);
			out_.emplace_back(Packet{ out.String() });
		}

		asio::awaitable<void> ReadRoutine()
		{
			while (socket_.is_open())
			{
				auto packet = co_await AsyncReadPacket(socket_);
				zs::StringReader ar(std::string{ packet.Data(),packet.Size() });
				auto tag = zs::Read<std::string>(ar);
				if (std::holds_alternative<zs::Error>(tag))
					co_return;

				auto iter = listeners_.find(std::get<0>(tag));
				if (iter == listeners_.end())
					continue;

				auto content = zs::Read<std::string>(ar);
				if (std::holds_alternative<zs::Error>(content))
					co_return;

				iter->second(std::move(std::get<0>(content)));
			}
		}

		asio::awaitable<void> WriteRoutine()
		{
			while (socket_.is_open())
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
		std::unordered_map<std::string, std::function<void(std::string&&)>> listeners_;
		std::function<void()> errorHandler_;// TODO
		std::list<Packet> out_;
	};
}