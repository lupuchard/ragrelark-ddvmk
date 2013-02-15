#ifndef DUNGEONSTACK_H
#define DUNGEONSTACK_H

#include <cstdarg>

#include "Generator.h"
#include "MobSpawner.h"

void setMobSpawner(MobSpawner* mobSpawner);

class DungeonStack {
    public:
        DungeonStack(string name, int depth, GenType genType);
        virtual ~DungeonStack();

        void setLight(int light, float change = 0.f);
        void setDifficulty(int diff, float change = 0.f);
        void setEnvironments(int numEnvironments, short* environments);
        void setNumStairs(int stairs, float change = 0.f);
        void setDimensions(int width, int height, float widthChange = 0.f, float heightChange = 0.f);
        void setEnemyDensity(float enemyPerSquare, float change);
        void setItemDensity(float itemsPerSquare, float change);
        void setTileset(Tile** tileset);

        void createZones();
        void genLevel(int level, vector<pair<Unit*, Zone*> >* unitsAdded);

        Zone* getZone(int depth);
        int getDepth();

        pair<short, short> addEntryStairs(); //creates stairs on the top floor and gives you their location
        int getNumStairs(int floor); //floor refers to the floor the stairs go down from (0 is the top floor)
        pair<short, short> getStairLoc(int floor, int index);
    private:
        Zone** zones;
        unsigned char* skeleton;
        int depth;
        int depthReached;
        GenType genType;
        vector<short> environments;
        string name;

        Tile** tileset;

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
        short** stairXs;
        short** stairYs;
};

#endif // DUNGEONSTACK_H
