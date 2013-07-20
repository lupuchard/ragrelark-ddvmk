/*
 *  Copyright 2013 Luke Puchner-Hardman
 *
 *  This file is part of Ragrelark.
 *  Ragrelark is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ragrelark is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ragrelark.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    delete tiledLoader;
    Ability::clean();
    folders.clear();
    formulas.clear();
    deleteData();
    cleanFov();
}
