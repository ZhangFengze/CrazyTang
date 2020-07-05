#pragma once
#include <asio.hpp>

namespace ct
{
	class Connection
	{
	public:
		Connection(asio::ip::tcp::socket&& socket);
		void AsyncWritePacket(const char* data, size_t size);

	private:
		void AsyncReadPacket();

		void OnData(const char* data, size_t size);
		void OnError(const std::error_code&);

	private:
		std::shared_ptr<bool> valid_ = std::make_shared<bool>(true);
		asio::ip::tcp::socket socket_;
		std::array<char, 1024 * 1024> buffer_;
	};
}
