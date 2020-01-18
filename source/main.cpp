#include "Renderer.h"
#include "Game.h"
#include "Timer.h"
int main(int argc, char* argv[])
{
	{
		auto rs = Renderer::Create("CrazyTang", 800, 600).value();
		Game game;
		Timer timer;

		SDL_Event e;
		bool quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
					quit = true;
			}

			float dt = timer.Elapsed();
			timer.Reset();

			game.Update(dt);

			rs.Clear();
			game.Render(rs);
			rs.Commit();
		}
	}

	return 0;
}