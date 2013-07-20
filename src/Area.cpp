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

void Area::setName(String n) {
    name = n;
}

SpeLoc Area::moveZones(Zone* z, Coord c) {
    Coord zc;
    if (c.x < 0) {
        zc.x = -1;
    } else if (c.x >= z->getWidth()) {
        zc.x = 1;
        c.x = 0;
    }
    if (c.y < 0) {
        zc.y = -1;
    } else if (c.y >= z->getHeight()) {
        zc.y = 1;
        c.y = 0;
    }
    Zone* newZone = NULL;
    for (unsigned int i = 0; i < zones.size(); i++) {
        if (zones[i] == z) {
            int t = i + zc.index(width);
            if (t < 0 || (unsigned int)t >= zones.size()) {
                SpeLoc nope;
                nope.loc.x = -1;
                return nope;
            }
            newZone = zones[t];
            break;
        }
    }
    if (newZone)  {
        if (c.x < 0) c.x = newZone->getWidth() - 1;
        if (c.y < 0) c.y = newZone->getHeight() - 1;
        SpeLoc newSpeLoc = {c, newZone};
        return newSpeLoc;
    }
    SpeLoc nope;
    nope.loc.x = -1;
    return nope;
}

SpeLoc Area::moveConnection(Zone* zone, Coord pos) {
    for (unsigned int i = 0; i < connections.size(); i++) {
        Connection c = connections[i];
        if (c.z1 == zone && c.loc1 == pos) {
            SpeLoc newSpeLoc = {c.loc2, c.z2};
            if (c.loc2.x == -2) connections.erase(connections.begin() + i);
            return newSpeLoc;
        } else if (c.z2 == zone && c.loc2 == pos) {
            SpeLoc newSpeLoc = {c.loc1, c.z1};
            if (c.loc1.x == -2) connections.erase(connections.begin() + i);
            return newSpeLoc;
        }
    }
    SpeLoc nope;
    nope.loc.x = -1;
    return nope;
}

void Area::changeConnection(Zone* zone, Coord prevLoc, Coord newLoc) {
    for (unsigned int i = 0; i < connections.size(); i++) {
        Connection* c = &connections[i];
        if (c->z1 == zone && c->loc1 == prevLoc) {
            c->loc1 = newLoc;
        } else if (c->z2 == zone && c->loc2 == prevLoc) {
            c->loc2 = newLoc;
        }
    }
}

void Area::addConnection(Connection c) {
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
