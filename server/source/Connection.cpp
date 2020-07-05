#include "Connection.h"

namespace ct
{
	Connection::Connection(asio::ip::tcp::socket&& socket)
		:socket_(std::move(socket))
	{
		AsyncReadPacket();
	}

	void Connection::AsyncWritePacket(const char* data, size_t size)
	{
		std::weak_ptr<bool> valid = valid_;

		auto buffer = std::make_shared<std::vector<char>>(size + 4);

		uint32_t* length = reinterpret_cast<uint32_t*>(buffer->data());
		*length = (uint32_t)size;

		char* content = buffer->data() + 4;
		std::copy(data, data + size, content);

		asio::async_write(socket_, asio::buffer(*buffer),
			[this, valid, buffer](const asio::error_code& error, size_t size)
		{
			if (valid.expired())
				return;
			if (error)
				return OnError(error);
		});
	}

	void Connection::AsyncReadPacket()
	{
		std::weak_ptr<bool> valid = valid_;
		asio::async_read(socket_, asio::buffer(buffer_, 4),
			[valid, this](const asio::error_code& error, size_t size)
		{
			if (valid.expired())
				return;
			if (error)
				return OnError(error);

			uint32_t length = *reinterpret_cast<uint32_t*>(buffer_.data());
			if (length > buffer_.size())
				return OnError({ asio::error::no_buffer_space });

			asio::async_read(socket_, asio::buffer(buffer_, length),
				[valid, this](const asio::error_code& error, size_t size)
			{
				if (valid.expired())
					return;
				if (error)
					return OnError(error);

				OnData(buffer_.data(), size);
				AsyncReadPacket();
			});
		});
	}

	void Connection::OnData(const char* data, size_t size)
	{
		AsyncWritePacket(data, size);
	}

	void Connection::OnError(const std::error_code&)
	{
		valid_.reset();
	}
}