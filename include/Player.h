#include "Unit.h"
#include "Area.h"

#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    unsigned char* bottomTex;
    unsigned char* bottomLoc;
    unsigned char* topTex;
    unsigned char* topLoc;
} zoneMemory;

class Player {
    public:
        Player();
        virtual ~Player();
        Zone* getZone();
        Area* getArea();
        Unit* getUnit();
        void setZone(Zone* z);
        void setArea(Area* a);
        void setUnitProto(StatHolder* p);
        void setName(string name);
        pair<int, int> getMemoryBottom(int x, int y);
        pair<int, int> getMemoryTop(int x, int y);
        void setMemory(int x, int y, unsigned char bt, unsigned char bl, unsigned char tt, unsigned char tl);
    protected:
    private:
        string name;
        Unit* unit;
        Zone* zone;
        Area* area;
        map<Zone*, zoneMemory*> memoryBank;
        zoneMemory* currentZoneMemory;
};

#endif // PLAYER_H
