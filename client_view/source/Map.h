#pragma once
#include <string>
#include <entityx/entityx.h>

namespace ct
{
	bool LoadMap(entityx::EntityManager&, const std::string& path);
}
