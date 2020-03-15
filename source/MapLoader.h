#pragma once
#include <string>
#include <entityx/entityx.h>
#include <filesystem>

namespace ct
{
	bool LoadMap(entityx::EntityManager&, const std::filesystem::path& path);
}
