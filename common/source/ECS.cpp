#include "ECS.h"

namespace ct
{
	EntityHandle EntityContainer::Create()
	{
		auto e = EntityHandle();
		e.id_ = ++assignedID_;
		e.container_ = this;

		entities_[e.id_] = {};
		return e;
	}

	bool EntityContainer::Has(uint64_t id) const
	{
		return entities_.find(id) != entities_.end();
	}
}
