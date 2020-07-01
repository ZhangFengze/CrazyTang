#include "Connection.h"

namespace ct
{
	Connection::Connection(asio::ip::tcp::socket&& socket)
		:socket_(std::move(socket))
	{
		ReadLength();
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

	void Connection::ReadLength()
	{
		std::weak_ptr<bool> valid = valid_;
		asio::async_read(socket_, asio::buffer(lengthBuffer_),
			[this, valid](const asio::error_code& error, size_t size)
		{
			if (valid.expired())
				return;
			if (error)
				return OnError();

			uint32_t length = 0;
			length = *reinterpret_cast<uint32_t*>(lengthBuffer_.data());
			ReadContent(length);
		});
	}

	void Connection::ReadContent(uint32_t length)
	{
		std::weak_ptr<bool> valid = valid_;
		asio::async_read(socket_, asio::buffer(contentBuffer_, length),
			[this, valid](const asio::error_code& error, size_t size)
		{
			if (valid.expired())
				return;
			if (error)
				return OnError();

			OnData(contentBuffer_.data(), size);
			ReadLength();
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