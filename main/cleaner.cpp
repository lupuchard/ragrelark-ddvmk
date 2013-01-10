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
    for (map<unsigned int, RandItemType*>::iterator i = tiledItems.begin(); i != tiledItems.end(); i++) {
        delete i->second;
    }
    tiledItems.clear();
    tiledTiles.clear();
    cleanSpawnData();
    folders.clear();
    formulas.clear();
    //delete [] unitStats; TODO fix
    deleteData();
    cleanFov();
}
