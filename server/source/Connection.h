#pragma once
#include <asio.hpp>

namespace ct
{
	class Connection
	{
	public:
		Connection(asio::ip::tcp::socket&& socket);
		void Write(const char* data, size_t size);

	private:
		void ReadLength();
		void ReadContent(uint32_t length);

		void OnData(const char* data, size_t size);
		void OnError();

	private:
		std::shared_ptr<bool> valid_ = std::make_shared<bool>(true);
		asio::ip::tcp::socket socket_;
		std::array<char, 4> lengthBuffer_;
		std::array<char, 1024 * 1024> contentBuffer_;
	};
}
