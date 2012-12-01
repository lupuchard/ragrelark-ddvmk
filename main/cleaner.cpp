#include "Start.h"

/*

cleans

 */

void Start::cleanup() {
    SDL_FreeSurface(display);
    IMG_Quit();
    SDL_Quit();
    delete player;
    delete world;
    //delete display;
    delete primeFolder;
    cleanSpawnData();
    folders.clear();
    formulas.clear();
    //delete [] unitStats; TODO fix
    deleteData();
    cleanFov();
}
