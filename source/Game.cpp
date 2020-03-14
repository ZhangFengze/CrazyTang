#include "Game.h"
#include "Math.h"
#include "Sprite.h"
#include "AnimatedSprite.h"
#include "Render.h"
#include "Background.h"
#include "Camera.h"
#include "Transformable.h"
#include "Move.h"

namespace ct
{
	Game::Game(sf::RenderTarget& target)
	{
		m_EntityX.systems.add<BackgroundSystem>();
		m_EntityX.systems.add<CameraSystem>(target);
		m_EntityX.systems.add<RenderSystem>(target);
		m_EntityX.systems.add<MoveSystem>();
		m_EntityX.systems.configure();

		Vector2f cameraSize{ 640,400 };

		CreateBackground(m_EntityX.entities, "../../../asset/environment/back.png", 2.f, 0.05f, -200.f, cameraSize.x());
		CreateBackground(m_EntityX.entities, "../../../asset/environment/middle.png", 1.f, 0.1f, 0.f, cameraSize.x());

		{
			auto e = m_EntityX.entities.create();
			auto anim = e.assign<AnimatedSprite>(
				AnimatedSprite(0.2f,
					{
						"../../../asset/sprites/player/idle/player-idle-1.png",
						"../../../asset/sprites/player/idle/player-idle-2.png",
						"../../../asset/sprites/player/idle/player-idle-3.png",
						"../../../asset/sprites/player/idle/player-idle-4.png"
					}
				)
			);
			e.assign<Transformable>()->position = { 0,0 };
			e.assign<Move>()->speed = 600.f;

			e.assign<Camera>()->size = cameraSize;
		}
	}

	void Game::Update(float dt)
	{
		m_EntityX.systems.update_all(dt);
	}
}