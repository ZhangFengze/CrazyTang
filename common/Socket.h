#pragma once
#include "Packet.h"
#include <asio.hpp>
#include <array>
#include <list>
#include <vector>
#include <functional>

namespace ct
{
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
