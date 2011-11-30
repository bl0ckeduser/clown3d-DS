#ifdef PC_TARGET

#include <SDL/SDL.h>

Uint32 ticks, start_ticks;

void LimitFPS(int fps)
{
	start_ticks = SDL_GetTicks();
	ticks = SDL_GetTicks();

	while(ticks-start_ticks<1000/fps)
		ticks = SDL_GetTicks();

}

#endif
