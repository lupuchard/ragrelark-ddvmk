#include "Start.h"

/* Initiates the SDL window and opengl stuffs. */
bool Start::init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }
    if((display = SDL_SetVideoMode(WIN1_WIDTH, WIN1_HEIGHT + CWIN_HEIGHT, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) {
        return false;
    }
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    startRenderer();

    srand(time(NULL));

    initFieldOfView();
    return true;
}
