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

#ifndef DUNGEONSTACK_H
#define DUNGEONSTACK_H

#include <cstdarg>

#include "Generator.h"
#include "MobSpawner.h"

void setMobSpawner(MobSpawner* mobSpawner);

class DungeonStack: boost::noncopyable {
    public:
        DungeonStack(String name, int depth, GenType genType);
        DungeonStack(const DungeonStack& other);
        virtual ~DungeonStack();

        void setLight(int light, float change = 0.f);
        void setDifficulty(int diff, float change = 0.f);
        void addEnvironment(short environment);
        void setNumStairs(int stairs, float change = 0.f);
        void setDimensions(int width, int height, float widthChange = 0.f, float heightChange = 0.f);
        void setEnemyDensity(float enemyPerSquare, float change);
        void setItemDensity(float itemsPerSquare, float change);
        void setTileset(TileSet* tileSet);

        void createZones();
        void genLevel(int level, std::vector<std::pair<Unit*, Zone*> >* unitsAdded);

        Zone* getZone(int depth);
        int getDepth();

        Coord addEntryStairs(); //creates stairs on the top floor and gives you their location
        int getNumStairs(int floor); //floor refers to the floor the stairs go down from (0 is the top floor)
        Coord getStairLoc(int floor, int index);

        void save(std::ostream& saveData);
        DungeonStack(std::istream& saveData);
    private:
        Zone** zones;
        unsigned char* skeleton;
        int depth;
        int depthReached;
        GenType genType;
        std::vector<short> environments;
        String name;

        TileSet* tileset;

        int width;
        int height;
        float widthChange;
        float heightChange;

        int light;
        float lightChange;
        int level;
        float levelChange;

        float enemyPerSquare;
        float eDensityChange;
        float itemsPerSquare;
        float iDensityChange;

        int numStairs;
        float stairsChange;
        Coord** stairLocs;
};

#endif // DUNGEONSTACK_H
