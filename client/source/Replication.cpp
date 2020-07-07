#include "Replication.h"
#include "Move.h"
#include "Transformable.h"

using namespace std::placeholders;

namespace ct
{
	ReplicationSystem::ReplicationSystem(Net& net, MessageHandler& messageHandler)
		:net_(net), messageHandler_(messageHandler)
	{
		messageHandler_.Register("player_state", std::bind(&ReplicationSystem::OnPlayerState, this, _1));
	}

	ReplicationSystem::~ReplicationSystem()
	{
		messageHandler_.UnRegister("player_state");
	}

	void ReplicationSystem::update(entityx::EntityManager& entities,
		entityx::EventManager& events, entityx::TimeDelta dt)
	{
		entities.each<Move, Transformable>(
			[&](entityx::Entity entity, Move& move, Transformable& trans)
		{
			if (net_.Working())
			{
				std::ostringstream os;
				os << "player_state " << trans.position.x() << ' ' << trans.position.y();
				auto message = os.str();
				net_.Send(message.data(), message.size());
			}
		});
	}

	void ReplicationSystem::OnPlayerState(const std::string& message)
	{
		printf("%s\n", message.c_str());
	}
}
