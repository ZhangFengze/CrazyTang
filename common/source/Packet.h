#pragma once
#include <vector>

namespace ct
{
	struct Packet
	{
		Packet() = default;
		Packet(size_t);
		Packet(const char*, size_t);

		const char* Data() const;
		size_t Size() const;

		std::vector<char> buffer_;
	};

	bool operator==(const Packet& left, const Packet& right);
	bool operator!=(const Packet& left, const Packet& right);
}
