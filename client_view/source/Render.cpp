#include "Render.h"
#include "Sprite.h"
#include "AnimatedSprite.h"
#include "Animator.h"
#include "Transformable.h"

namespace
{
	using namespace ct;

	void UpdateSprite(sf::RenderTarget& target, Sprite& sprite, Transformable& trans, Vector2f extraScale)
	{
		sprite.sprite.setPosition(ToSfVector2f(trans.position));

		auto bounds = sprite.sprite.getGlobalBounds();
		sprite.sprite.setOrigin(bounds.width * sprite.anchor.x(), bounds.height * sprite.anchor.y());

		auto scale = extraScale.cwiseProduct(sprite.scale);
		sprite.sprite.setScale(ToSfVector2f(scale));

		target.draw(sprite.sprite);
	}

	void UpdateAnimatedSprite(sf::RenderTarget& target, AnimatedSprite& anim, Transformable& trans, Vector2f extraScale)
	{
		if (anim.sprites.empty())
			return;
		anim.accumulated++;

		auto index = anim.accumulated / anim.period;
		index = index % anim.sprites.size();

		auto& sprite = anim.sprites[index];
		sprite.anchor = anim.anchor;
		UpdateSprite(target, sprite, trans, extraScale.cwiseProduct(anim.scale));
	}

	void UpdateAnimator(sf::RenderTarget& target, Animator& anim, Transformable& trans, Vector2f extraScale)
	{
		auto iter = anim.states.find(anim.now);
		if (iter == anim.states.end())
			return;
		auto& animatedSprite = iter->second;
		animatedSprite.anchor = anim.anchor;
		UpdateAnimatedSprite(target, animatedSprite, trans, extraScale.cwiseProduct(anim.scale));
	}
}

namespace ct
{
	void RenderSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		entities.each<Sprite, Transformable>(
			[&](entityx::Entity entity, Sprite& sprite, Transformable& trans)
		{
			UpdateSprite(target_, sprite, trans, { 1,1 });
		});

		entities.each<AnimatedSprite, Transformable>([&](entityx::Entity entity, AnimatedSprite& anim, Transformable& trans)
		{
			UpdateAnimatedSprite(target_, anim, trans, { 1,1 });
		});

		entities.each<Animator, Transformable>([&](entityx::Entity entity, Animator& anim, Transformable& trans)
		{
			UpdateAnimator(target_, anim, trans, { 1,1 });
		});
	}
}

