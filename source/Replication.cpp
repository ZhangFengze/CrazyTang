#include "Replication.h"
#include "Move.h"
#include "Transformable.h"
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
	}

	void ReplicationSystem::receive(const DataEvent& event)
	{
	}

	void ReplicationSystem::receive(const DisconnectionEvent& event)
	{
	}
}