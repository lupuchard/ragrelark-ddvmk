#include "Area.h"


Area::Area(int w): StatHolder(V_AREA) {
    width = w;
}

Area::~Area() {
    for (unsigned int i = 0; i < zones.size(); i++) {
        delete zones[i];
    }
    zones.clear();
    for (unsigned int i = 0; i < dungeonStacks.size(); i++) {
        delete dungeonStacks[i];
    }
    dungeonStacks.clear();
}

void Area::addZone(Zone* z) {
    zones.push_back(z);
}

void Area::setName(string n) {
    name = n;
}

speLoc Area::moveZones(Zone* z, int x, int y) {
    int zx = 0;
    int zy = 0;
    if (x < 0) {
        zx = -1;
    } else if (x >= z->getWidth()) {
        zx = 1;
        x = 0;
    }
    if (y < 0) {
        zy = -1;
    } else if (y >= z->getHeight()) {
        zy = 1;
        y = 0;
    }
    Zone* newZone = NULL;
    for (unsigned int i = 0; i < zones.size(); i++) {
        if (zones[i] == z) {
            unsigned int t = i + zx + zy * width;
            if (t < 0 || t >= zones.size()) {
                speLoc nope;
                nope.x = -1;
                return nope;
            }
            newZone = zones[t];
            break;
        }
    }
    if (newZone)  {
        if (x < 0) x = newZone->getWidth() - 1;
        if (y < 0) y = newZone->getHeight() - 1;
        speLoc newSpeLoc = {(short)x, (short)y, newZone};
        return newSpeLoc;
    }
    speLoc nope;
    nope.x = -1;
    return nope;
}

speLoc Area::moveConnection(Zone* zone, int x, int y) {
    for (unsigned int i = 0; i < connections.size(); i++) {
        connection c = connections[i];
        if (c.z1 == zone && c.x1 == x && c.y1 == y) {
            speLoc newSpeLoc = {c.x2, c.y2, c.z2};
            if (c.x2 == -2) connections.erase(connections.begin() + i);
            return newSpeLoc;
        } else if (c.z2 == zone && c.x2 == x && c.y2 == y) {
            speLoc newSpeLoc = {c.x1, c.y1, c.z1};
            if (c.x1 == -2) connections.erase(connections.begin() + i);
            return newSpeLoc;
        }
    }
    speLoc nope;
    nope.x = -1;
    return nope;
}

void Area::changeConnection(Zone* zone, int x, int y, int newX, int newY) {
    for (unsigned int i = 0; i < connections.size(); i++) {
        connection* c = &connections[i];
        if (c->z1 == zone && c->x1 == x && c->y1 == y) {
            c->x1 = newX;
            c->y1 = newY;
        } else if (c->z2 == zone && c->x2 == x && c->y2 == y) {
            c->x2 = newX;
            c->y2 = newY;
        }
    }
}

void Area::addConnection(connection c) {
    connections.push_back(c);
}

unsigned int Area::getNumZones() {
    return zones.size();
}

Zone* Area::getZone(int z) {
    return zones[z];
}

void Area::addDungeonStack(DungeonStack* dungeonStack) {
    dungeonStacks.push_back(dungeonStack);
    for (int i = 0; i < dungeonStack->getDepth(); i++) {
        dungeonStack->getZone(i)->tagDungeon(dungeonStacks.size() - 1, i);
    }
}

unsigned int Area::getNumDungeonStacks() {
    return dungeonStacks.size();
}

DungeonStack* Area::getDungeonStack(int stackIndex) {
    return dungeonStacks[stackIndex];
}
