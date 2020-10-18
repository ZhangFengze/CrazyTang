#pragma once
#include <any>
#include <cassert>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include <functional>

namespace ct
{
	class EntityContainer;

	class EntityHandle
	{
	public:
		template<typename Component>
		Component* Add();

		template<typename Component>
		void Remove();

		template<typename Component>
		bool Has() const;

		template<typename Component>
		Component* Get();

		template<typename Component>
		const Component* Get() const;

	public:
		void Destroy();
		bool Valid() const;

	public:
		uint64_t id_ = 0;
		EntityContainer* container_ = nullptr;
	};

	bool operator<(EntityHandle, EntityHandle);
	bool operator==(EntityHandle, EntityHandle);
	bool operator!=(EntityHandle, EntityHandle);

	class EntityContainer
	{
	public:
		EntityHandle Create();
		void ForEach(std::function<void(EntityHandle)>);

	private:
		bool Has(uint64_t) const;
		void Remove(uint64_t);

	private:
		template<typename Component>
		Component* Add(uint64_t id);

		template<typename Component>
		void Remove(uint64_t id);

		template<typename Component>
		bool Has(uint64_t id);

		template<typename Component>
		Component* Get(uint64_t id);

	private:
		friend class EntityHandle;
		uint64_t assignedID_ = 0;
		using EntityComponents = std::unordered_map<std::type_index, std::any>;
		std::unordered_map<uint64_t, EntityComponents> entities_;
	};

	template<typename Component>
	Component* EntityHandle::Add()
	{
		return container_->Add<Component>(id_);
	}

	template<typename Component>
	void EntityHandle::Remove()
	{
		return container_->Remove<Component>(id_);
	}

	template<typename Component>
	bool EntityHandle::Has() const
	{
		return container_->Has<Component>(id_);
	}

	template<typename Component>
	Component* EntityHandle::Get()
	{
		return container_->Get<Component>(id_);
	}

	template<typename Component>
	const Component* EntityHandle::Get() const
	{
		return container_->Get<Component>(id_);
	}

	template<typename Component>
	Component* EntityContainer::Add(uint64_t id)
	{
		assert(Has(id));
		auto& components = entities_[id];

		assert(components.find(typeid(Component)) == components.end());
		auto type = std::type_index(typeid(Component));
		auto [where, _] = components.emplace(std::make_pair(type, Component{}));
		auto value = &(where->second);
		return std::any_cast<Component>(value);
	}

	template<typename Component>
	void EntityContainer::Remove(uint64_t id)
	{
		assert(Has(id));
		auto& components = entities_[id];

		assert(components.find(typeid(Component)) != components.end());
		auto type = std::type_index(typeid(Component));
		components.erase(type);
	}

	template<typename Component>
	bool EntityContainer::Has(uint64_t id)
	{
		assert(Has(id));
		auto& components = entities_[id];

		auto type = std::type_index(typeid(Component));
		return components.find(type) != components.end();
	}

	template<typename Component>
	Component* EntityContainer::Get(uint64_t id)
	{
		assert(Has(id));

		if (!Has<Component>(id))
			return nullptr;

		auto type = std::type_index(typeid(Component));
		auto& components = entities_[id];
		auto value = &(components[type]);
		return std::any_cast<Component>(value);
	}
}

namespace std
{
	template<>
	struct hash<ct::EntityHandle>
	{
		std::size_t operator()(ct::EntityHandle e) const noexcept
		{
			std::size_t h1 = std::hash<decltype(e.container_)>{}(e.container_);
			std::size_t h2 = std::hash<decltype(e.id_)>{}(e.id_);
			return h1 ^ (h2 << 1);
		}
	};
}