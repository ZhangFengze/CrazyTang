#include "Serializer.h"

namespace ct
{
	OutputArchive::OutputArchive(std::ostream& os)
		:os_(os)
	{
	}

	InputArchive::InputArchive(std::istream& is)
		:is_(is)
	{
	}
}
