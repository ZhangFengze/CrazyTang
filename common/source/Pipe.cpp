#include "Pipe.h"

namespace ct
{
	void MockPipe::OnPacket(std::function<void(Packet&&)> handler)
	{
		packetHandler_ = handler;
		if (!packetHandler_)
			return;
		ProcessPackets();
	}

	void MockPipe::SendPacket(Packet&& packet)
	{
		if (broken_)
			return;
		writtenPackets_.emplace_back(std::move(packet));
	}

	void MockPipe::SendPacket(const Packet& packet)
	{
		auto copy = packet;
		return SendPacket(std::move(copy));
	}

	void MockPipe::OnBroken(std::function<void(void)> handler)
	{
		brokenHandler_ = handler;
	}

	bool MockPipe::IsBroken() const
	{
		return broken_;
	}

	void MockPipe::PacketArrive(const Packet& packet)
	{
		if (broken_)
			return;
		receivedPackets_.emplace_back(packet);
		ProcessPackets();
	}

	void MockPipe::SetBroken()
	{
		if (broken_)
			return;
		broken_ = true;
		if (brokenHandler_)
			brokenHandler_();
	}

	void MockPipe::ProcessPackets()
	{
		if (broken_)
			return;
		if (!packetHandler_)
			return;
		while (packetHandler_ && (!receivedPackets_.empty()))
		{
			auto packet = std::move(receivedPackets_.front());
			receivedPackets_.pop_front();
			packetHandler_(std::move(packet));
		}
	}

	void PairedPipe::OnPacket(std::function<void(Packet&&)> handler)
	{
		pipe_.OnPacket(handler);
	}

	void PairedPipe::SendPacket(Packet&& packet)
	{
		auto copy = packet;
		pipe_.SendPacket(std::move(packet));
		auto mate = mate_.lock();
		if (mate)
			mate->pipe_.PacketArrive(copy);
	}

	void PairedPipe::SendPacket(const Packet& packet)
	{
		auto copy = packet;
		SendPacket(std::move(copy));
	}

	void PairedPipe::OnBroken(std::function<void(void)> handler)
	{
		pipe_.OnBroken(handler);
	}

	bool PairedPipe::IsBroken() const
	{
		return pipe_.IsBroken();
	}

	std::pair<std::shared_ptr<PairedPipe>, std::shared_ptr<PairedPipe>> PairedPipe::CreatePair()
	{
		auto pipe0 = std::make_shared<PairedPipe>();
		auto pipe1 = std::make_shared<PairedPipe>();

		pipe0->mate_ = pipe1;
		pipe1->mate_ = pipe0;
		return { pipe0, pipe1 };
	}
}
