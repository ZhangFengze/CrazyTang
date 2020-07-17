#pragma once
#include <functional>
#include <system_error>
#include "Socket.h"

namespace ct
{
	template<typename Socket>
	class Pipe
	{
	public:
		Pipe(Socket&&);

	public:
		void OnPacket(std::function<void(const char*, size_t)> handler);
		void SendPacket(const char*, size_t);

		void OnBroken(std::function<void(void)>);
		bool IsBroken() const;

	private:
		void Receive();
		void Send();
		void ProcessPackets();

		std::shared_ptr<std::vector<char>> MakePacket(const char*, size_t);

		void OnReceivedPacket();
		void OnSetPacketHandler();
		void OnError(const std::error_code&);

	public:
		std::shared_ptr<bool> alive_ = std::make_shared<bool>(false);
		Socket socket_;

		std::function<void(const char*, size_t)> packetHandler_;

		std::list<std::shared_ptr<std::vector<char>>> in_;
		bool receiving_ = false;

		std::list<std::shared_ptr<std::vector<char>>> out_;
		bool sending_ = false;

		std::function<void()> brokenHandler_;
		bool broken_ = false;
	};

	template<typename Socket>
	Pipe<Socket>::Pipe(Socket&& socket)
		:socket_(std::move(socket))
	{
	}

	template<typename Socket>
	void Pipe<Socket>::OnPacket(std::function<void(const char*, size_t)> handler)
	{
		packetHandler_ = handler;
		if (!packetHandler_)
			return;
		OnSetPacketHandler();
	}

	template<typename Socket>
	void Pipe<Socket>::OnBroken(std::function<void(void)> handler)
	{
		brokenHandler_ = handler;
	}

	template<typename Socket>
	bool Pipe<Socket>::IsBroken() const
	{
		return broken_;
	}

	template<typename Socket>
	void Pipe<Socket>::SendPacket(const char* data, size_t size)
	{
		if (broken_)
			return;
		out_.push_back(MakePacket(data, size));
		Send();
	}

	template<typename Socket>
	void Pipe<Socket>::Receive()
	{
		if (broken_ || receiving_)
			return;
		receiving_ = true;

		std::weak_ptr<bool> alive = alive_;
		socket_.AsyncReadPacket(
			[alive, this](const std::error_code& error, const char* data, size_t size)
		{
			if (alive.expired())
				return;
			if (error)
				return OnError(error);
			in_.push_back(MakePacket(data, size));
			receiving_ = false;
			OnReceivedPacket();
		});
	}

	template<typename Socket>
	void Pipe<Socket>::Send()
	{
		if (broken_ || sending_ || out_.empty())
			return;
		sending_ = true;

		std::weak_ptr<bool> alive = alive_;
		socket_.AsyncWritePacket(out_.front()->data(), out_.front()->size(),
			[alive, this](const std::error_code& error)
		{
			if (alive.expired())
				return;
			if (error)
				return OnError(error);
			sending_ = false;
			Send();
		});
		out_.pop_front();
	}

	template<typename Socket>
	void Pipe<Socket>::ProcessPackets()
	{
		if (broken_ || !packetHandler_)
			return;
		while (packetHandler_ && (!in_.empty()))
		{
			packetHandler_(in_.front()->data(), in_.front()->size());
			in_.pop_front();
		}
	}

	template<typename Socket>
	std::shared_ptr<std::vector<char>> Pipe<Socket>::MakePacket(const char* data, size_t size)
	{
		auto packet = std::make_shared<std::vector<char>>(size);
		std::copy(data, data + size, packet->data());
		return packet;
	}

	template<typename Socket>
	void Pipe<Socket>::OnReceivedPacket()
	{
		if (broken_ || !packetHandler_)
			return;
		ProcessPackets();
		Receive();
	}

	template<typename Socket>
	void Pipe<Socket>::OnSetPacketHandler()
	{
		if (broken_)
			return;
		Receive();

		if (in_.empty())
			return;
		std::weak_ptr<bool> alive = alive_;
		asio::defer([alive, this]()
		{
			if (alive.expired())
				return;
			ProcessPackets();
		});
	}

	template<typename Socket>
	void Pipe<Socket>::OnError(const std::error_code& error)
	{
		if (broken_)
			return;
		broken_ = true;
		if (brokenHandler_)
			brokenHandler_();
	}
}
