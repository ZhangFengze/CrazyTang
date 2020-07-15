#pragma once
#include <asio.hpp>

namespace ct
{
	class Connection
	{
	public:
		Connection(asio::ip::tcp::socket&& socket);

		void AsyncWritePacket(const char* data, size_t size,
			std::function<void(const std::error_code&)>&&);

		void AsyncReadPacket(std::function<void(const std::error_code&, const char*, size_t)>&& handler);

	public:
		std::shared_ptr<bool> valid_ = std::make_shared<bool>(true);
		asio::ip::tcp::socket socket_;
		std::array<char, 1024 * 1024> buffer_;
	};
}
