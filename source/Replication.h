#pragma once
#include <entityx/entityx.h>
#include "Net.h"

namespace ct
{
    struct ReplicationSystem 
        :public entityx::System<ReplicationSystem>, public entityx::Receiver<ReplicationSystem>
	{
		ReplicationSystem(Net&);
        void configure(entityx::EventManager& events) override;
		void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

        void receive(const ConnectionEvent&);
        void receive(const DataEvent&);
        void receive(const DisconnectionEvent&);

		Net& net_;
	};
}
