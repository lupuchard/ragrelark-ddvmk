#include "World.h"

World::World(): StatHolder(V_WORLD) {
    theTime = 0;
}

World::~World() {
    delete[] worldMap;
}

void World::addArea(Area* a) {
    areas.push_back(a);
}

void World::setDim(int wid, int hei) {
    width = wid;
    height = hei;
    worldMap = new Location[width * height];
}
