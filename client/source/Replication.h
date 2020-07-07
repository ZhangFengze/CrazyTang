#pragma once
#include <entityx/entityx.h>
#include "Net.h"
#include "MessageHandler.h"

namespace ct
{
    struct ReplicationSystem :public entityx::System<ReplicationSystem>
    {
        ReplicationSystem(Net&, MessageHandler&);
        ~ReplicationSystem() override;

        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
        void OnPlayerState(const std::string&);

        Net& net_;
        MessageHandler& messageHandler_;
    };
}
