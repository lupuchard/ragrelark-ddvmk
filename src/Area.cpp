#include "Area.h"


Area::Area(int w, int h): StatHolder(V_AREA) {
    width = w;
    height = h;
}

Area::~Area() {
    delete[] zones;
}

void Area::setZones(Zone** z, int n) {
    zones = z;
    numZones = n;
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
    for (int i = 0; i < numZones; i++) {
        if (zones[i] == z) {
            int t = i + zx + zy * width;
            if (t < 0 || t >= numZones) {
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
            return newSpeLoc;
        } else if (c.z2 == zone && c.x2 == x && c.y2 == y) {
            speLoc newSpeLoc = {c.x1, c.y1, c.z1};
            return newSpeLoc;
        }
    }
    speLoc nope;
    nope.x = -1;
    return nope;
}

void Area::addConnection(connection c) {
    connections.push_back(c);
}

unsigned short Area::getNumZones() {
    return numZones;
}

Zone* Area::getZone(int z) {
    return zones[z];
}
