#include "MapLoader.h"
#include "Sprite.h"
#include "Transformable.h"
#include "Collider.h"
#include <Tileson.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace
{
	std::pair<fs::path, sf::IntRect> ExtractTile(tson::Map& map, int tileID)
	{
		for (const auto& tileSet : map.getTilesets())
		{
			int firstID = tileSet.getFirstgid();
			int lastID = firstID + tileSet.getTileCount();
			if (tileID >= firstID && tileID <= lastID)
			{
				int index = tileID - firstID;

				int x = index % tileSet.getColumns();
				int y = index / tileSet.getColumns();

				sf::IntRect rect;
				rect.left = x * tileSet.getTileSize().x;
				rect.top = y * tileSet.getTileSize().y;
				rect.width = tileSet.getTileSize().x;
				rect.height = tileSet.getTileSize().y;

				return { tileSet.getImage(),rect };
			}
		}
		return { "",{} };
	}
}

namespace ct
{
	bool LoadMap(entityx::EntityManager& entities, const fs::path& path)
	{
		tson::Tileson t;
		auto map = t.parse(path);
		if (map.getStatus() != tson::Map::ParseStatus::OK)
			return false;

		auto tileSize = map.getTileSize();

		for (auto& layer : map.getLayers())
		{
			for (auto& chunk : layer.getChunks())
			{
				for (int y = chunk.getPosition().y; y < chunk.getPosition().y + chunk.getSize().y; ++y)
				{
					for (int x = chunk.getPosition().x; x < chunk.getPosition().x + chunk.getSize().x; ++x)
					{
						int index = y * chunk.getSize().x + x;
						int tileID = chunk.getData()[index];
						if (tileID == 0)
							continue;
						auto [tileSet, rect] = ExtractTile(map, tileID);

						auto e = entities.create();
						auto sprite = e.assign<Sprite>(path/".."/tileSet);
						sprite->sprite.setTextureRect(rect);
						Vector2f pos = { x * tileSize.x, y * tileSize.y };
						e.assign<Transformable>()->position = pos;

						auto tile = map.getTileMap().at(tileID);
						auto objectGroup = tile->getObjectgroup();
						for (auto& obj : objectGroup.getObjects())
						{
							if (obj.getObjectType() != tson::Object::Type::Rectangle)
								continue;
							if (obj.getType() != "box collider")
								continue;
							auto box = e.assign<Box>();
							box->size = { obj.getSize().x,obj.getSize().y };
							box->topLeft = pos + Vector2f{ obj.getPosition().x, obj.getPosition().y };
						}
					}
				}
			}
		}

		return true;
	}
}
