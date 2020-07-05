#include "Connection.h"

namespace ct
{
	Connection::Connection(asio::ip::tcp::socket&& socket)
		:socket_(std::move(socket))
	{
		AsyncReadPacket();
	}

	void Connection::Write(const char* data, size_t size)
	{
		std::weak_ptr<bool> valid = valid_;

		auto buffer = std::make_shared<std::vector<char>>(size + 4);

		uint32_t* lengthPointer = reinterpret_cast<uint32_t*>(buffer->data());
		*lengthPointer = size;

		char* contentPointer = buffer->data() + 4;
		std::copy(data, data + size, contentPointer);

		asio::async_write(socket_, asio::buffer(*buffer),
			[this, valid, buffer](const asio::error_code& error, size_t size)
		{
			if (valid.expired())
				return;
			if (error)
				return OnError();
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
				return OnError();

			uint32_t length = *reinterpret_cast<uint32_t*>(buffer_.data());
			if (length > buffer_.size())
				return OnError();

			asio::async_read(socket_, asio::buffer(buffer_, length),
				[valid, this](const asio::error_code& error, size_t size)
			{
				if (valid.expired())
					return;
				if (error)
					return OnError();

				OnData(buffer_.data(), size);
				AsyncReadPacket();
			});
		});
	}

	void Connection::OnData(const char* data, size_t size)
	{
		Write(data, size);
	}

	void Connection::OnError()
	{
		valid_.reset();
	}
}