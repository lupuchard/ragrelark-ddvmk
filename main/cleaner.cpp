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
    delete mobSpawner;
    delete primeFolder;
    for (map<unsigned int, RandItemType*>::iterator i = tiledItems.begin(); i != tiledItems.end(); i++) {
        delete i->second;
    }
    cleanAbilities();
    tiledItems.clear();
    tiledTiles.clear();
    folders.clear();
    formulas.clear();
    deleteData();
    cleanFov();
}
