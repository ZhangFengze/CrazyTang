#include "Map.h"
#include "Sprite.h"
#include "Transformable.h"
#include "Collider.h"
#include <Tileson.h>

namespace
{
	using namespace ct;

	bool InTileSet(int tileID, const tson::Tileset& tileSet)
	{
		int firstID = tileSet.getFirstgid();
		int lastID = firstID + tileSet.getTileCount();
		return tileID >= firstID && tileID <= lastID;
	}

	std::pair<int, int> TilePosition(int tileID, const tson::Tileset& tileSet)
	{
		int index = tileID - tileSet.getFirstgid();
		int x = index % tileSet.getColumns();
		int y = index / tileSet.getColumns();
		return { x,y };
	}

	std::pair<std::string, sf::IntRect> TileTexture(tson::Map& map, int tileID)
	{
		for (const auto& tileSet : map.getTilesets())
		{
			if (!InTileSet(tileID, tileSet))
				continue;

			auto [x, y] = TilePosition(tileID, tileSet);

			sf::IntRect rect;
			rect.left = x * tileSet.getTileSize().x;
			rect.top = y * tileSet.getTileSize().y;
			rect.width = tileSet.getTileSize().x;
			rect.height = tileSet.getTileSize().y;

			return { tileSet.getImage().string(),rect };
		}
		return { "",{} };
	}

	int TileID(const tson::Chunk& chunk, int x, int y)
	{
		int index = y * chunk.getSize().x + x;
		return chunk.getData()[index];
	}
	
	Vector2f TilePosition(const tson::Chunk& chunk, int x, int y, Vector2i tileSize)
	{
		x += chunk.getPosition().x;
		y += chunk.getPosition().y;
		return { x * tileSize.x(),y * tileSize.y() };
	}

	void LoadObject(const tson::Object& object, entityx::Entity entity, Vector2f position)
	{
		if (object.getObjectType() != tson::Object::Type::Rectangle)
			return;
		if (object.getType() != "box collider")
			return;
		if (entity.has_component<Box>())
			return;

		auto box = entity.assign<Box>();
		box->size = { object.getSize().x,object.getSize().y };
		box->topLeft = position + Vector2f{ object.getPosition().x, object.getPosition().y };
	}

	void LoadChunk(entityx::EntityManager& entities, const std::string& mapPath, tson::Map& map, const tson::Chunk& chunk)
	{
		Vector2i tileSize{ map.getTileSize().x,map.getTileSize().y };

		for (int y = 0; y < chunk.getSize().y; ++y)
		{
			for (int x = 0; x < chunk.getSize().x; ++x)
			{
				int tileID = TileID(chunk, x, y);
				if (tileID == 0)
					continue;

				auto e = entities.create();

				auto pos = TilePosition(chunk, x, y, tileSize);
				e.assign<Transformable>(pos);

				auto [texture, rect] = TileTexture(map, tileID);
				e.assign<Sprite>(mapPath + "/../" + texture, rect);

				auto tile = map.getTileMap().at(tileID);
				auto objectGroup = tile->getObjectgroup();
				for (const auto& object : objectGroup.getObjects())
					LoadObject(object, e, pos);
			}
		}
	}
}

namespace ct
{
	bool LoadMap(entityx::EntityManager& entities, const std::string& path)
	{
		tson::Tileson t;
		auto map = t.parse(path);
		if (map.getStatus() != tson::Map::ParseStatus::OK)
			return false;

		for (auto& layer : map.getLayers())
			for (auto& chunk : layer.getChunks())
				LoadChunk(entities, path, map, chunk);

		return true;
	}
}
