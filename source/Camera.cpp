#include "Camera.h"
#include <optional>

namespace ct
{
	void CameraSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		std::optional<Camera> camera;
		es.each<Camera>([&camera](entityx::Entity entity, Camera& c)
		{
			camera = c;
		});
		if (!camera)
			return;
		sf::View view = target_.getView();
		view.setCenter(ToSfVector2f(camera->position));
		view.setSize(ToSfVector2f(camera->size));
		target_.setView(view);
	}
}
