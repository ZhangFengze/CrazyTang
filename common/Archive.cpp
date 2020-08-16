#include "Archive.h"

namespace ct
{
	OutputArchive::OutputArchive(std::ostream& os)
		:os_(os)
	{
	}

	OutputStringArchive::OutputStringArchive()
		: ar_(os_)
	{
	}

	std::string OutputStringArchive::String() const
	{
		return os_.str();
	}

	InputArchive::InputArchive(std::istream& is)
		:is_(is)
	{
	}

	InputStringArchive::InputStringArchive(const std::string& str)
		: is_(str), ar_(is_)
	{
	}
}
