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

#ifndef ZONE_H
#define ZONE_H

#include "Location.h"
#include "Tile.h"
#include "StatHolder.h"
#include "Swarmer.h"

#define MAX_ZONE_SIZE 4096
#define MAX_ZONE_WIDHEI 256

enum GenType{GEN_NONE = 0, GEN_MYDUN = 1, GEN_BSP = 2, GEN_DUNGEON = 10};

//takes up 16 bytes when not filled

class Zone: public StatHolder {
    public:
        Zone(String name, int width, int height, int lightness, bool fill);
        virtual ~Zone();
        int getWidth();
        int getHeight();
        unsigned char getLightness();
        String getName();
        Location* getLocationAt(Coord pos);
        Location* getLocationAt(int i);
        Location* safeGetLocationAt(Coord pos);

        void fillTiles(int* tiles);
        void fillHeights(int* heights);
        void fillStructs(int* structs);

        void fill();
        bool isFilled();

        void tagDungeon(int index, int depth);
        std::pair<int, int> dungeonTag();

        int getIndex();

        void save(std::ostream& saveFile);
        void saveUnit(Unit* unit, std::ostream& saveData);
        Zone(std::istream& saveData);
        Unit* loadUnit(std::istream& saveData);
    private:
        Location* locs;
        bool filled;
        unsigned char lightness; //0 - 11, 0 = blind, 1 - 10 = normal, 11 = surface
        unsigned short width;
        unsigned short height;
        String name;
        unsigned short ind;

        char stackIndex;
        unsigned char stackDepth;
};

#endif // ZONE_H
