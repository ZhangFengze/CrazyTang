#ifndef CT_TEST

int main()
{
	return 0;
}

#else

#include <catch.hpp>

int main(int argc, char* argv[])
{
	return Catch::Session().run(argc, argv);
}

#endif