#include "Entity.h"

namespace ct
{
	void EntityHandle::Destroy()
	{
		return container_->Remove(id_);
	}

	bool EntityHandle::Valid() const
	{
		if (!container_)
			return false;
		return container_->Has(id_);
	}

	bool operator<(EntityHandle lhs, EntityHandle rhs)
	{
		if (lhs.container_ < rhs.container_)
			return true;
		if (lhs.container_ > rhs.container_)
			return false;
		return lhs.id_ < rhs.id_;
	}

	bool operator==(EntityHandle lhs, EntityHandle rhs)
	{
		return lhs.container_ == rhs.container_ && lhs.id_ == rhs.id_;
	}

	bool operator!=(EntityHandle lhs, EntityHandle rhs)
	{
		return !(lhs == rhs);
	}

	EntityHandle EntityContainer::Create()
	{
		auto e = EntityHandle();
		e.id_ = ++assignedID_;
		e.container_ = this;

		entities_[e.id_] = {};
		return e;
	}

	void EntityContainer::ForEach(std::function<void(EntityHandle)> func)
	{
		for (const auto& [id, components] : entities_)
		{
			auto e = EntityHandle{};
			e.id_ = id;
			e.container_ = this;
			func(e);
		}
	}

	void EntityContainer::Remove(uint64_t id)
	{
		assert(Has(id));
		entities_.erase(id);
	}

	bool EntityContainer::Has(uint64_t id) const
	{
		return entities_.find(id) != entities_.end();
	}
}
