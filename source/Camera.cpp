#include "Camera.h"
#include "Transformable.h"
#include <optional>

namespace ct
{
	void CameraSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		es.each<Camera,Transformable>(
			[this](entityx::Entity entity, Camera& camera, Transformable& trans)
		{
			if (!camera.target.valid())
				return;
			auto targetPosition = camera.target.component<Transformable>()->position;
			trans.position = targetPosition + Vector2f{ 0,-100 };

			sf::View view = target_.getView();
			view.setCenter(ToSfVector2f(trans.position));
			view.setSize(ToSfVector2f(camera.size));
			target_.setView(view);
		});
	}
}
