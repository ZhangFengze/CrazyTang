#pragma once
#include <map>
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

        bool HasPeer(const std::string&) const;

		Net& net_;
        std::map<std::string, entityx::Entity> peers_;
	};
}
