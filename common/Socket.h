#pragma once
#include "Packet.h"
#include <asio.hpp>
#include <array>
#include <list>
#include <vector>
#include <functional>

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

	class Socket
	{
	public:
		Socket(asio::ip::tcp::socket&& socket);

		void AsyncWritePacket(Packet&& packet, std::function<void(const std::error_code&)>&&);
		void AsyncWritePacket(const Packet& packet, std::function<void(const std::error_code&)>&&);

		void AsyncReadPacket(std::function<void(const std::error_code&, Packet&&)>&& handler);

	public:
		asio::ip::tcp::socket socket_;
		std::array<uint32_t, 1> lengthBuffer_;
		std::array<char, 1024 * 1024> buffer_;
	};

	struct StubSocket
	{
		void AsyncWritePacket(Packet&& packet, std::function<void(const std::error_code&)>&&) {}
		void AsyncWritePacket(const Packet& packet, std::function<void(const std::error_code&)>&&) {}

		void AsyncReadPacket(std::function<void(const std::error_code&, Packet&&)>&& handler) {}
	};

	class MockSocket
	{
	public:
		MockSocket(asio::io_context& io);

		void AsyncWritePacket(Packet&& packet, std::function<void(const std::error_code&)>&&);
		void AsyncWritePacket(const Packet& packet, std::function<void(const std::error_code&)>&&);

		void AsyncReadPacket(std::function<void(const std::error_code&, Packet&&)>&& handler);

		void PacketArrive(const std::error_code& error, const Packet& packet);

		std::vector<Packet> writtenPackets;
		std::list<std::pair<std::error_code, Packet>> receivedPackets;

		std::function<void(const std::error_code&, Packet&&)> pendingReader;

		asio::io_context& io_;
	};

}
