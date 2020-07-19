#pragma once
#include <vector>

namespace ct
{
	struct Packet
	{
		Packet() = default;
		Packet(const Packet&) = default;
		Packet(Packet&&) = default;
		Packet(size_t);
		Packet(const char*, size_t);
		Packet& operator=(const Packet&) = default;
		Packet& operator=(Packet&&) = default;

		const char* Data() const;
		size_t Size() const;

		std::vector<char> buffer_;
	};

	bool operator==(const Packet& left, const Packet& right);
	bool operator!=(const Packet& left, const Packet& right);
}
