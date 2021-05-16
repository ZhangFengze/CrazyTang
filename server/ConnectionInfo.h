#pragma once
#include <cstdint>
#include <memory>
#include "../common/NetAgent.h"

namespace ct
{
	struct ConnectionInfo
	{
		uint64_t connectionID;
		std::weak_ptr<ct::NetAgent<CoroPipe>> agent;
	};

	struct Connection
	{
		uint64_t connectionID;
		std::shared_ptr<ct::NetAgent<CoroPipe>> agent;
	};
} // namespace