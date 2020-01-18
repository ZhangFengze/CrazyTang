#include "Input.h"
#include <cassert>
#include <SDL.h>

namespace
{
	bool IsKeyDown(SDL_Scancode scanCode)
	{
		auto state = SDL_GetKeyboardState(nullptr);
		return state[scanCode];
	}
}

bool Input::IsKeyDown(Key key)
{
	switch (key)
	{
	case Key::Up:
		return ::IsKeyDown(SDL_SCANCODE_UP);
	case Key::Down:
		return ::IsKeyDown(SDL_SCANCODE_DOWN);
	case Key::Left:
		return ::IsKeyDown(SDL_SCANCODE_LEFT);
	case Key::Right:
		return ::IsKeyDown(SDL_SCANCODE_RIGHT);
	default:
		assert(false);
		return false;
	}
}
