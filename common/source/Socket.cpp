#include "Socket.h"

namespace ct
{
	Socket::Socket(asio::ip::tcp::socket&& socket)
		:socket_(std::move(socket))
	{
	}

	void Socket::AsyncWritePacket(const char* data, size_t size,
		std::function<void(const std::error_code&)>&& handler)
	{
		auto buffer = std::make_shared<std::vector<char>>(size + 4);

		uint32_t* length = reinterpret_cast<uint32_t*>(buffer->data());
		*length = (uint32_t)size;

		char* content = buffer->data() + 4;
		std::copy(data, data + size, content);

		asio::async_write(socket_, asio::buffer(*buffer),
			[buffer, handler](const asio::error_code& error, size_t size)
		{
			handler(error);
		});
	}

	void Socket::AsyncReadPacket(std::function<void(const std::error_code&, const char*, size_t)>&& handler)
	{
		std::weak_ptr<bool> valid = valid_;
		asio::async_read(socket_, asio::buffer(buffer_, 4),
			[valid, this, handler](const asio::error_code& error, size_t size)
		{
			if (valid.expired())
				return handler(asio::error::make_error_code(asio::error::operation_aborted), nullptr, 0);
			if (error)
				return handler(error, nullptr, 0);

			uint32_t length = *reinterpret_cast<uint32_t*>(buffer_.data());
			if (length > buffer_.size())
				return handler(asio::error::make_error_code(asio::error::no_buffer_space), nullptr, 0);

			asio::async_read(socket_, asio::buffer(buffer_, length),
				[valid, this, handler](const asio::error_code& error, size_t size)
			{
				if (valid.expired())
					return;
				if (error)
					return handler(error, nullptr, 0);
				handler(error, buffer_.data(), size);
			});
		});
	}
}