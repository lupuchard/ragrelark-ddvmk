#include <time.h>
#include <iostream>
#include <ncurses.h>
#include <pthread.h>
#include <SDL/SDL.h>
#include "Area.h"
#include "World.h"
#include "Player.h"
#include "renderer.h"
#include "unitHandler.h"

#define FPS 30

bool init();
void events();
void logic();
void cleanup();

void movePlayer(int dir);

bool done;
SDL_Surface* display;
SDL_Event event;

Player* player;
World* world;

int main() {
    World testWorld(10, 10);
    Unit playerUnit;
    Zone startZone(10, 10, 2);
    int heights[100] = {4, 5, 6, 5, 3, 2, 1, 0, 0, 0,
                        5, 6, 7, 0, 1, 1, 1, 0, 0, 0,
                        6, 7, 8,11,12,13,14,15, 8, 0,
                        7, 8, 9,10,25,26,27,16, 0, 0,
                        8, 9,10, 9,24,31,28,17, 0, 0,
                        0, 0,10, 8,23,30,29,18, 0, 0,
                        0, 0, 8, 7,22,21,20,19, 0, 0,
                        0, 0, 8, 6, 5, 4, 3, 2, 1, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    startZone.fillHeights(&heights[0]);
    Player p(&startZone, &playerUnit);

    world = &testWorld;
    player = &p;

    done = !init();
    while(!done) {
        events();
        logic();
        render();
    }
    cout << "done" << endl;
    cleanup();
    return 0;
}

bool init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }
    if((display = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL)) == NULL) {
        return false;
    }
    startRenderer(player);
    return true;
}



void logic() {
}

void cleanup() {
    SDL_FreeSurface(display);
    SDL_Quit();
    delete player;
    delete world;
    delete display;
}

void movePlayer(int direction) {
    moveUnit(player->getUnit(), player->getZone(), direction);
}
