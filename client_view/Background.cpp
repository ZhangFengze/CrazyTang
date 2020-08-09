#include "Background.h"
#include "Sprite.h"
#include "Camera.h"
#include "Transformable.h"

namespace ct
{
	void BackgroundSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		Vector2f cameraSize;
		Vector2f cameraPosition;
		es.each<Camera, Transformable>(
			[&](entityx::Entity entity, Camera& camera, Transformable& trans)
		{
			cameraSize = camera.size;
			cameraPosition = trans.position;
		});

		es.each<Background>(
			[&](entityx::Entity entity, Background& bg)
		{
			float cameraLeftBound = cameraPosition.x() - cameraSize.x() / 2.f;
			float relative = (cameraLeftBound * bg.moveSpeed) / bg.width;
			relative = relative - std::floor(relative);

			Vector2f pos = { cameraLeftBound - relative * bg.width, cameraPosition.y() + bg.y };

			for (auto& sprite : bg.sprites)
			{
				sprite.component<Transformable>()->position = pos;
				pos += Vector2f{ bg.width,0.f };
			}
		});
	}

	entityx::Entity CreateBackground(entityx::EntityManager& entities,
		const std::string& path, Vector2f scale, float moveSpeed, float y, float cameraWidth)
	{
		auto entity = entities.create();
		auto bg = entity.assign<Background>();
		bg->moveSpeed = moveSpeed;
		bg->y = y;

		auto spriteEntity = entities.create();
		spriteEntity.assign<Transformable>();
		auto sprite = spriteEntity.assign<Sprite>(path);
		sprite->scale = scale;
		bg->width = sprite->sprite.getGlobalBounds().width * scale.x();

		bg->sprites.push_back(spriteEntity);
		while (bg->sprites.size() < (cameraWidth / bg->width) + 1)
			bg->sprites.push_back(entities.create_from_copy(spriteEntity));

		return entity;
	}
}
