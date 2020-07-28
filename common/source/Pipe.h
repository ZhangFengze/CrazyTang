#pragma once
#include <functional>
#include <system_error>
#include <utility>
#include <memory>
#include "Packet.h"
#include "Socket.h"

namespace ct
{
	template<typename Socket = ct::Socket>
	class Pipe
	{
	public:
		Pipe(Socket&&);

	public:
		void OnPacket(std::function<void(Packet&&)> handler);
		void SendPacket(Packet&&);
		void SendPacket(const Packet&);

		void OnBroken(std::function<void(void)>);
		bool IsBroken() const;

	private:
		void Receive();
		void Send();
		void ProcessPackets();

		void OnReceivedPacket();
		void OnSetPacketHandler();
		void OnError(const std::error_code&);

	public:
		std::shared_ptr<bool> alive_ = std::make_shared<bool>(false);
		Socket socket_;

		std::function<void(Packet&&)> packetHandler_;

		std::list<Packet> in_;
		bool receiving_ = false;

		std::list<Packet> out_;
		bool sending_ = false;

		std::function<void()> brokenHandler_;
		bool broken_ = false;
	};

	class MockPipe
	{
	public:
		void OnPacket(std::function<void(Packet&&)> handler);
		void SendPacket(Packet&&);
		void SendPacket(const Packet&);

		void OnBroken(std::function<void(void)>);
		bool IsBroken() const;

	public:
		void PacketArrive(const Packet&);
		void SetBroken();

	private:
		void ProcessPackets();

	public:
		std::function<void(Packet&&)> packetHandler_;
		std::list<Packet> receivedPackets_;

		std::function<void(void)> brokenHandler_;
		bool broken_ = false;

		std::vector<Packet> writtenPackets_;
	};

	class PairedPipe
	{
	public:
		void OnPacket(std::function<void(Packet&&)> handler);
		void SendPacket(Packet&&);
		void SendPacket(const Packet&);

		void OnBroken(std::function<void(void)>);
		bool IsBroken() const;

	public:
		static std::pair<std::shared_ptr<PairedPipe>,
			std::shared_ptr<PairedPipe>> CreatePair();

	private:
		std::weak_ptr<PairedPipe> mate_;
		MockPipe pipe_;
	};

	template<typename Socket>
	Pipe<Socket>::Pipe(Socket&& socket)
		:socket_(std::move(socket))
	{
	}

	template<typename Socket>
	void Pipe<Socket>::OnPacket(std::function<void(Packet&&)> handler)
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
	void Pipe<Socket>::SendPacket(Packet&& packet)
	{
		if (broken_)
			return;
		out_.emplace_back(std::move(packet));
		Send();
	}

	template<typename Socket>
	void Pipe<Socket>::SendPacket(const Packet& packet)
	{
		auto copy = packet;
		return SendPacket(std::move(copy));
	}

	template<typename Socket>
	void Pipe<Socket>::Receive()
	{
		if (broken_ || receiving_)
			return;
		receiving_ = true;

		std::weak_ptr<bool> alive = alive_;
		socket_.AsyncReadPacket(
			[alive, this](const std::error_code& error, Packet&& packet)
		{
			if (alive.expired())
				return;
			if (error)
				return OnError(error);
			in_.emplace_back(std::move(packet));
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
		socket_.AsyncWritePacket(std::move(out_.front()),
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
			auto packet = std::move(in_.front());
			in_.pop_front();
			packetHandler_(std::move(packet));
		}
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
