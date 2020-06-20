#include "Replication.h"
#include "Game.h"
#include "Move.h"
#include "Transformable.h"
#include "AnimatedSprite.h"
#include <vector>
#include <iostream>

namespace ct
{
	ReplicationSystem::ReplicationSystem(Net& net)
		:net_(net)
	{
	}

	void ReplicationSystem::configure(entityx::EventManager& events)
	{
		events.subscribe<ConnectionEvent>(*this);
		events.subscribe<DataEvent>(*this);
		events.subscribe<DisconnectionEvent>(*this);
	}

	void ReplicationSystem::update(entityx::EntityManager& entities,
		entityx::EventManager& events, entityx::TimeDelta dt)
	{
		entities.each<Move, Transformable>(
			[&](entityx::Entity entity, Move& move, Transformable& trans)
		{
			std::ostringstream os;
			os << trans.position.x() << ' ' << trans.position.y();
			net_.Broadcast(os.str());
		});
	}

	void ReplicationSystem::receive(const ConnectionEvent& event)
	{
		assert(!HasPeer(event.from));
		auto e = game->entities.create();

		auto idle = AnimatedSprite(12,
			{
				"../../../asset/sprites/player/idle/player-idle-1.png",
				"../../../asset/sprites/player/idle/player-idle-2.png",
				"../../../asset/sprites/player/idle/player-idle-3.png",
				"../../../asset/sprites/player/idle/player-idle-4.png"
			}
		);
		idle.anchor = { 0.5f,1.f };
		e.assign<AnimatedSprite>(idle);

		e.assign<Transformable>();

		peers_[event.from] = e;
	}

	void ReplicationSystem::receive(const DataEvent& event)
	{
		assert(HasPeer(event.from));
		std::istringstream is(event.data);
		float x, y;
		is >> x >> y;
		peers_[event.from].component<Transformable>()->position = { x,y };
	}

	void ReplicationSystem::receive(const DisconnectionEvent& event)
	{
		assert(HasPeer(event.from));
		peers_[event.from].destroy();
		peers_.erase(event.from);
	}

	bool ReplicationSystem::HasPeer(const std::string& peer) const
	{
		return peers_.find(peer) != peers_.end();
	}
}