/*
 * Screen blanker for Pandora, used to clear X screen before running
 * frame buffer applications.
 * 
 * Steve Maddison, August 2010
 *
 * Released into the public domain.
 */

#include <SDL.h>

int main( int argc, char *argv[] ) {
	SDL_Surface *screen;
	SDL_Event event;

	SDL_ShowCursor(SDL_DISABLE);

	screen = SDL_SetVideoMode( 800, 480, 16, SDL_FULLSCREEN );

	if( !screen )
		return -1;

	SDL_FillRect( screen, NULL, SDL_MapRGB(screen->format,0,0,0) );

	for(;;) {
		while( SDL_PollEvent( &event ) ) {
			if( event.type == SDL_QUIT ) {
				return 0;
			}
		}
		SDL_Delay(100);
	}
	
	SDL_Quit();
}

