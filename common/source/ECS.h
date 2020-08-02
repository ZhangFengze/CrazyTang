#pragma once
#include <any>
#include <typeinfo>
#include <typeindex>

namespace ct
{
	class Entity
	{
	public:
		template<typename Component>
		Component* Add();

		template<typename Component>
		void Remove();

		template<typename Component>
		bool Has();

		template<typename Component>
		Component* Get();

	private:
		std::unordered_map<std::type_index, std::any> components_;
	};

	template<typename Component>
	Component* Entity::Add()
	{
		assert(components_.find(typeid(Component)) == components_.end());
		auto type = std::type_index(typeid(Component));
		auto [where, _] = components_.emplace(std::make_pair(type, Component{}));
		auto value = &(where->second);
		return std::any_cast<Component>(value);
	}

	template<typename Component>
	void Entity::Remove()
	{
		assert(components_.find(typeid(Component)) != components_.end());
		auto type = std::type_index(typeid(Component));
		components_.erase(type);
	}

	template<typename Component>
	bool Entity::Has()
	{
		auto type = std::type_index(typeid(Component));
		return components_.find(type) != components_.end();
	}

	template<typename Component>
	Component* Entity::Get()
	{
		if (!Has<Component>())
			return nullptr;
		auto type = std::type_index(typeid(Component));
		auto value = &(components_[type]);
		return std::any_cast<Component>(value);
	}
}
