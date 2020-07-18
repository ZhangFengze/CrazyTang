#include "Pipe.h"

namespace ct
{
	void MockPipe::OnPacket(std::function<void(const char*, size_t)> handler)
	{
		packetHandler_ = handler;
		if (!packetHandler_)
			return;
		ProcessPackets();
	}

	void MockPipe::SendPacket(const char* data, size_t size)
	{
		if (broken_)
			return;
		writtenPackets_.push_back(std::string{ data,size });
	}

	void MockPipe::OnBroken(std::function<void(void)> handler)
	{
		brokenHandler_ = handler;
	}

	bool MockPipe::IsBroken() const
	{
		return broken_;
	}

	void MockPipe::PacketArrive(const char* data, size_t size)
	{
		if (broken_)
			return;
		receivedPackets_.push_back(std::string{ data,size });
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
			auto packet = receivedPackets_.front();
			receivedPackets_.pop_front();
			packetHandler_(packet.data(), packet.size());
		}
	}
}
