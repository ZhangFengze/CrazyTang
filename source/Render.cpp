#include "Render.h"
#include "Sprite.h"
#include "AnimatedSprite.h"

namespace ct
{
	void RenderSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		es.each<Sprite>([this](entityx::Entity entity, Sprite& sprite)
		{
			target_.draw(sprite.sprite);
		});

		es.each<AnimatedSprite>([this, dt](entityx::Entity entity, AnimatedSprite& anim)
		{
			if (anim.sprites.empty())
				return;
			anim.accumulated += dt;

			size_t index = std::floor(anim.accumulated / anim.interval);
			index = index % anim.sprites.size();
			target_.draw(anim.sprites[index].sprite);
		});
	}
}

