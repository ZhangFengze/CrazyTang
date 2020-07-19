#include "Packet.h"

namespace ct
{
	Packet::Packet(size_t size)
		:buffer_(size)
	{
	}

	Packet::Packet(const char* data, size_t size)
		:buffer_(data, data + size)
	{
	}

	const char* Packet::Data() const
	{
		return buffer_.data();
	}

	size_t Packet::Size() const
	{
		return buffer_.size();
	}

	bool operator==(const Packet& left, const Packet& right)
	{
		if (left.Size() != right.Size())
			return false;
		return 0 == memcmp(left.Data(), right.Data(), left.Size());
	}

	bool operator!=(const Packet& left, const Packet& right)
	{
		return !(left == right);
	}
}
