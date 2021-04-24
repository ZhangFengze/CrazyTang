#pragma once
#include <cstdint>
#include <memory>
#include "../common/NetAgent.h"

namespace ct
{
	struct ConnectionInfo
	{
		uint64_t connectionID;
		std::weak_ptr<ct::NetAgent<>> agent;
	};
} // namespace