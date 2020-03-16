#include "Game.h"
#include "Math.h"
#include "Sprite.h"
#include "AnimatedSprite.h"
#include "Animator.h"
#include "Render.h"
#include "Background.h"
#include "Camera.h"
#include "Transformable.h"
#include "Move.h"
#include "Map.h"
#include "Texture.h"

namespace
{
	using namespace ct;
	entityx::Entity CreatePlayer(entityx::EntityManager& entities)
	{
		auto e = entities.create();

		auto idleRight = AnimatedSprite(12,
			{
				"../../../asset/sprites/player/idle/player-idle-1.png",
				"../../../asset/sprites/player/idle/player-idle-2.png",
				"../../../asset/sprites/player/idle/player-idle-3.png",
				"../../../asset/sprites/player/idle/player-idle-4.png"
			}
		);
		auto idleLeft = idleRight;
		idleLeft.scale = { -1,1 };

		auto runRight = AnimatedSprite(12,
			{
				"../../../asset/sprites/player/run/player-run-1.png",
				"../../../asset/sprites/player/run/player-run-2.png",
				"../../../asset/sprites/player/run/player-run-3.png",
				"../../../asset/sprites/player/run/player-run-4.png",
				"../../../asset/sprites/player/run/player-run-5.png",
				"../../../asset/sprites/player/run/player-run-6.png",
			}
		);
		auto runLeft = runRight;
		runLeft.scale = { -1,1 };

		auto crouchRight = AnimatedSprite(12,
			{
				"../../../asset/sprites/player/crouch/player-crouch-1.png",
				"../../../asset/sprites/player/crouch/player-crouch-2.png",
			}
		);
		auto crouchLeft = crouchRight;
		crouchLeft.scale = { -1,1 };

		auto jumpRiseRight = AnimatedSprite(12,
			{
				"../../../asset/sprites/player/jump/player-jump-1.png",
			}
		);
		auto jumpRiseLeft = jumpRiseRight;
		jumpRiseLeft.scale = { -1,1 };

		auto jumpFallRight = AnimatedSprite(12,
			{
				"../../../asset/sprites/player/jump/player-jump-2.png",
			}
		);
		auto jumpFallLeft = jumpFallRight;
		jumpFallLeft.scale = { -1,1 };

		auto animator = e.assign<Animator>();
		animator->states =
		{
			{"idle-right",idleRight},
			{"idle-left",idleLeft},
			{"run-right",runRight},
			{"run-left",runLeft},
			{"crouch-right",crouchRight},
			{"crouch-left",crouchLeft},
			{"jump-rise-right",jumpRiseRight},
			{"jump-rise-left",jumpRiseLeft},
			{"jump-fall-right",jumpFallRight},
			{"jump-fall-left",jumpFallLeft},
		};
		animator->now = "idle-right";
		animator->anchor = { 0.5f,1.f };

		e.assign<Transformable>();

		auto move = e.assign<Move>();
		move->acceleration = 0.1f;
		move->brakeAcceleration = 0.2f;
		move->speed = 2.f;

		move->jumpInitialYSpeed = 4.f;
		move->jumpGravity = 0.2f;

		move->size = { 16,16 };

		return e;
	}

	entityx::Entity CreateFollowCamera(entityx::EntityManager& entities, entityx::Entity player, Vector2f cameraSize)
	{
		auto e = entities.create();
		e.assign<Transformable>();
		auto camera = e.assign<Camera>();
		camera->size = cameraSize;
		camera->target = player;
		return e;
	}
}

namespace ct
{
	Game::Game()
		:events(_.events),
		systems(_.systems),
		entities(_.entities)
	{
	}

	void Game::Run()
	{
		sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "CrazyTang", sf::Style::Default);

		systems.add<TextureLoader>();
		systems.add<BackgroundSystem>();
		systems.add<RenderSystem>(window);
		systems.add<MoveSystem>();
		systems.add<CameraSystem>(window);
		systems.configure();

		Vector2f cameraSize{ 640,400 };

		CreateBackground(entities, "../../../asset/environment/back.png", { 2.f,2.f }, 0.05f, -200.f, cameraSize.x());
		CreateBackground(entities, "../../../asset/environment/middle.png", { 1.f,1.f }, 0.1f, -0.f, cameraSize.x());

		LoadMap(entities, "../../../asset/level/map.json");
		auto player = CreatePlayer(entities);
		CreateFollowCamera(entities, player, cameraSize);
		
		sf::Clock clock;
		sf::Time accumulated = sf::seconds(0);
		sf::Time frame = sf::seconds(1.f / 60.f);
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


			accumulated += clock.restart();
			while (accumulated > frame)
			{
				window.clear();
				systems.update_all(frame.asSeconds());
				accumulated -= frame;
				window.display();
			}
		}
	}
}
