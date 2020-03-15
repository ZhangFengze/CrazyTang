#include "Game.h"
#include "Math.h"
#include "Sprite.h"
#include "AnimatedSprite.h"
#include "Render.h"
#include "Background.h"
#include "Camera.h"
#include "Transformable.h"
#include "Move.h"
#include "MapLoader.h"
#include "Texture.h"

namespace
{
	using namespace ct;
	entityx::Entity CreatePlayer(entityx::EntityManager& entities)
	{
		auto e = entities.create();

		auto idle = AnimatedSprite(0.2f,
			{
				"../../../asset/sprites/player/idle/player-idle-1.png",
				"../../../asset/sprites/player/idle/player-idle-2.png",
				"../../../asset/sprites/player/idle/player-idle-3.png",
				"../../../asset/sprites/player/idle/player-idle-4.png"
			}
		);
		auto run = AnimatedSprite(0.2f,
			{
				"../../../asset/sprites/player/run/player-run-1.png",
				"../../../asset/sprites/player/run/player-run-2.png",
				"../../../asset/sprites/player/run/player-run-3.png",
				"../../../asset/sprites/player/run/player-run-4.png",
				"../../../asset/sprites/player/run/player-run-5.png",
				"../../../asset/sprites/player/run/player-run-6.png",
			}
		);

		e.assign<AnimatedSprite>(run)->anchor = { 0.5f,1.f };
		e.assign<Transformable>()->position = { 0,0 };

		auto move = e.assign<Move>();
		move->speed = 200.f;
		move->size = { 16,16 };

		return e;
	}
}

namespace ct
{
	Game::Game()
		:events(m_EntityX.events),
		systems(m_EntityX.systems),
		entities(m_EntityX.entities)
	{
	}

	void Game::Run()
	{
		sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "CrazyTang", sf::Style::Default);

		m_EntityX.systems.add<TextureLoader>();
		m_EntityX.systems.add<BackgroundSystem>();
		m_EntityX.systems.add<CameraSystem>(window);
		m_EntityX.systems.add<RenderSystem>(window);
		m_EntityX.systems.add<MoveSystem>();
		m_EntityX.systems.configure();

		Vector2f cameraSize{ 640,400 };

		CreateBackground(m_EntityX.entities, "../../../asset/environment/back.png", 2.f, 0.05f, -200.f, cameraSize.x());
		CreateBackground(m_EntityX.entities, "../../../asset/environment/middle.png", 1.f, 0.1f, -0.f, cameraSize.x());

		LoadMap(m_EntityX.entities, "../../../asset/level/map.json");
		auto player = CreatePlayer(m_EntityX.entities);

		{
			auto e = m_EntityX.entities.create();
			e.assign<Transformable>();
			auto camera = e.assign<Camera>();
			camera->size = cameraSize;
			camera->target = player;
		}
		
		sf::Clock clock;
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
					window.close();
			}

			auto dt = clock.restart();

			window.clear();
			m_EntityX.systems.update_all(dt.asSeconds());
			window.display();
		}
	}
}
