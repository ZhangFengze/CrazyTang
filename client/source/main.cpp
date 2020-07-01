#ifndef CT_TEST

#include "Game.h"

int main(int argc, char* argv[])
{
	ct::Game _game;
	ct::game = &_game;
	_game.Run();
	ct::game = nullptr;
	return 0;
}

#else

#include <catch.hpp>

int main(int argc, char* argv[])
{
	return Catch::Session().run(argc, argv);
}

#endif