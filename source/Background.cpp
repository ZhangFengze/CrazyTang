#include "Background.h"
#include "Sprite.h"
#include "Camera.h"

namespace ct
{
	void BackgroundSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		es.each<Camera, Background, Sprite>([this](entityx::Entity entity, Camera& camera, Background& bg, Sprite& sprite)
		{
			auto delta = (camera.position - bg.originPosition).cwiseProduct(bg.moveSpeed);
			auto pos = bg.originPosition + delta;
			sprite.sprite.setPosition(ToSfVector2f(pos));
		});
	}
}
