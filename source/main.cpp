#include "Renderer.h"
#include "Game.h"
int main(int argc, char* argv[])
{
	{
		auto rs = Renderer::Create("CrazyTang", 800, 600).value();
		Game game;

		SDL_Event e;
		bool quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&e) != 0)
			{
				if (e.type == SDL_QUIT)
					quit = true;
			}
			game.Update();

			rs.Clear();
			game.Render(rs);
			rs.Commit();

			SDL_Delay(1);
		}
	}

	return 0;
}