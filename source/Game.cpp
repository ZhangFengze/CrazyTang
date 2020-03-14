#include "Game.h"
#include "Math.h"
#include "Sprite.h"
#include "AnimatedSprite.h"
#include "Render.h"
#include "Background.h"
#include "Camera.h"
#include "Move.h"

namespace ct
{
	Game::Game(sf::RenderTarget& target)
	{
		m_EntityX.systems.add<BackgroundSystem>();
		m_EntityX.systems.add<RenderSystem>(target);
		m_EntityX.systems.add<CameraSystem>(target);
		m_EntityX.systems.add<MoveSystem>();
		m_EntityX.systems.configure();

		{
			auto e = m_EntityX.entities.create();
			e.assign<Background>(Background{ Vector2f{ 0,0 }, Vector2f{ 0.01f,0 } });
			e.assign<Sprite>("../../../asset/environment/back.png");
		}

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
		}

		auto camera = m_EntityX.entities.create();
		auto data = camera.assign<Camera>();
		data->position = { 160,100 };
		data->size = { 320,200 };
	}

	void Game::Update(float dt)
	{
		m_EntityX.systems.update_all(dt);
	}
}
