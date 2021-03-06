#pragma once
#include <cstdint>
#include <memory>
#include "../common/Net.h"

namespace ct
{
	struct ConnectionInfo
	{
		uint64_t connectionID;
		std::weak_ptr<ct::NetAgent> agent;
	};

	struct Connection
	{
		uint64_t connectionID;
		std::shared_ptr<ct::NetAgent> agent;
	};
} // namespace