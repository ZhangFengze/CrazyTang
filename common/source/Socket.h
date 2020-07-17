#pragma once
#include <asio.hpp>

namespace ct
{
	class Socket
	{
	public:
		Socket(asio::ip::tcp::socket&& socket);

		void AsyncWritePacket(const char* data, size_t size,
			std::function<void(const std::error_code&)>&&);

		void AsyncReadPacket(std::function<void(const std::error_code&, const char*, size_t)>&& handler);

	public:
		std::shared_ptr<bool> valid_ = std::make_shared<bool>(true);
		asio::ip::tcp::socket socket_;
		std::array<char, 1024 * 1024> buffer_;
	};

	struct StubSocket
	{
		void AsyncWritePacket(const char* data, size_t size,
			std::function<void(const std::error_code&)>&& handler) {}

		void AsyncReadPacket(std::function<void(const std::error_code&, const char*, size_t)>&& handler) {}
	};

	class MockSocket
	{
	public:
		MockSocket(asio::io_context& io);

		void AsyncWritePacket(const char* data, size_t size,
			std::function<void(const std::error_code&)>&& handler);

		void AsyncReadPacket(std::function<void(const std::error_code&, const char*, size_t)>&& handler);

		void PacketArrive(const std::error_code& error, const char* data, size_t size);

		std::vector<std::string> writtenPackets;
		std::list<std::pair<std::error_code, std::string>> receivedPackets;

		std::function<void(const std::error_code&, const char*, size_t)> pendingReader;

		asio::io_context& io_;
	};

}
