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
    //delete display;
    delete primeFolder;
    for (map<unsigned int, RandItemType*>::iterator i = tiledItems.begin(); i != tiledItems.end(); i++) {
        delete i->second;
    }
    for (map<string, MobEquipSet*>::iterator i = mobEquipsMap.begin(); i != mobEquipsMap.end(); i++) {
        delete i->second;
    }
    cleanAbilities();
    tiledItems.clear();
    tiledTiles.clear();
    folders.clear();
    formulas.clear();
    //delete [] unitStats; TODO fix
    deleteData();
    cleanFov();
}
