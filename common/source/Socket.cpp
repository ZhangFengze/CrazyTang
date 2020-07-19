#include "Socket.h"

namespace ct
{
	Socket::Socket(asio::ip::tcp::socket&& socket)
		:socket_(std::move(socket))
	{
	}

	void Socket::AsyncWritePacket(Packet&& packet,
		std::function<void(const std::error_code&)>&& handler)
	{
		auto data = std::make_shared<Packet>(std::move(packet));
		lengthBuffer_[0] = static_cast<uint32_t>(data->Size());

		auto buffers = std::make_shared<std::vector<asio::const_buffer>>();
		buffers->push_back(asio::buffer(lengthBuffer_));
		buffers->push_back(asio::buffer((const void*)data->Data(), data->Size()));

		asio::async_write(socket_, *buffers,
			[data, buffers, handler](const asio::error_code& error, size_t size)
		{
			handler(error);
		});
	}

	void Socket::AsyncWritePacket(const Packet& packet, std::function<void(const std::error_code&)>&& handler)
	{
		auto copy = packet;
		return AsyncWritePacket(std::move(copy), std::move(handler));
	}

	void Socket::AsyncReadPacket(std::function<void(const std::error_code&, Packet&&)>&& handler)
	{
		asio::async_read(socket_, asio::buffer(buffer_, 4),
			[this, handler](const asio::error_code& error, size_t size)
		{
			if (error)
				return handler(error, {});

			uint32_t length = *reinterpret_cast<uint32_t*>(buffer_.data());
			if (length > buffer_.size())
				return handler(asio::error::make_error_code(asio::error::no_buffer_space), {});

			asio::async_read(socket_, asio::buffer(buffer_, length),
				[this, handler](const asio::error_code& error, size_t size)
			{
				if (error)
					return handler(error, {});
				handler(error, { buffer_.data(), size });
			});
		});
	}

	MockSocket::MockSocket(asio::io_context& io)
		:io_(io)
	{}

	void MockSocket::AsyncWritePacket(Packet&& packet, std::function<void(const std::error_code&)> && handler)
	{
		asio::defer(io_, [=]()
		{
			writtenPackets.push_back(packet);
			handler(std::error_code{});
		});
	}

	void MockSocket::AsyncWritePacket(const Packet& packet, std::function<void(const std::error_code&)>&& handler)
	{
		auto copy = packet;
		return AsyncWritePacket(std::move(packet), std::move(handler));
	}

	void MockSocket::AsyncReadPacket(std::function<void(const std::error_code&, Packet&&)>&& handler)
	{
		if (receivedPackets.empty())
		{
			pendingReader = handler;
			return;
		}
		auto packet = std::move(receivedPackets.front());
		receivedPackets.pop_front();
		asio::defer(io_, [=]() mutable
		{
			handler(packet.first, std::move(packet.second));
		});
	}

	void MockSocket::PacketArrive(const std::error_code& error, const Packet& packet)
	{
		receivedPackets.emplace_back(error, packet);

		if (pendingReader)
		{
			auto packet = receivedPackets.front();
			receivedPackets.pop_front();
			auto reader = pendingReader;
			pendingReader = nullptr;
			reader(packet.first, std::move(packet.second));
		}
	}
}