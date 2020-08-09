#ifdef CT_TEST
#include <catch.hpp>
#include "Serializer.h"
#include <sstream>

using namespace std::literals;

namespace
{
}

TEST_CASE("archive basic types")
{
	std::ostringstream os;
	ct::OutputArchive o(os);

	o.Write(3);
	o.Write(5.3f);
	o.Write(8.0);
	o.Write("hi"sv);
	o.Write(std::string("hello"));

	std::istringstream is(os.str());
	ct::InputArchive i(is);

	REQUIRE(i.Read<int>() == 3);
	REQUIRE(i.Read<float>() == 5.3f);
	REQUIRE(i.Read<double>() == 8.0);
	REQUIRE(i.Read<std::string>() == "hi");
	REQUIRE(i.Read<std::string>() == "hello");
}

TEST_CASE("string archive basic types")
{
	ct::OutputStringArchive o;

	o.Write(3);
	o.Write(5.3f);
	o.Write(8.0);
	o.Write("hi"sv);
	o.Write(std::string("hello"));

	ct::InputStringArchive i(o.String());

	REQUIRE(i.Read<int>() == 3);
	REQUIRE(i.Read<float>() == 5.3f);
	REQUIRE(i.Read<double>() == 8.0);
	REQUIRE(i.Read<std::string>() == "hi");
	REQUIRE(i.Read<std::string>() == "hello");
}
#endif
