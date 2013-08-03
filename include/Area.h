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

#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>
#include "StatHolder.h"
#include "DungeonStack.h"

#define NUM_ESCAPE_DIRS 3

typedef struct {
    Coord loc1;
    Coord loc2;
    Zone* z1;
    Zone* z2;
} Connection;

typedef struct {
    Coord loc;
    Zone* z;
} SpeLoc;

void setAreaFormUser(FormulaUser* formUser);

class Area {
    public:
        Area(int width, String name);
        virtual ~Area();

        String getName();

        void addZone(Zone* zone);
        unsigned int getNumZones();
        Zone* getZone(int i);
        Zone* getZone(std::pair<int, int> dungeonTag);

        void addDungeonStack(DungeonStack* dungeonStack);
        unsigned int getNumDungeonStacks();
        DungeonStack* getDungeonStack(int stackIndex);

        void addConnection(Connection c);
        SpeLoc moveConnection(Zone* zone, Coord where);
        void changeConnection(Zone* zone, Coord prevLoc, Coord newLoc);
        SpeLoc moveZones(Zone* z, Coord where);

        void save(std::ostream& saveData);
        Area(std::istream& saveData);
    private:
        String name;
        std::vector<Zone*> zones;
        std::vector<DungeonStack*> dungeonStacks;
        std::vector<Connection> connections;
        int width;
};

#endif // AREA_H
