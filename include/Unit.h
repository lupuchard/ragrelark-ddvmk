#include <string>
#include "StatHolder.h"
#include "Stat.h"
#include "Tile.h"
#include "Item.h"
using namespace std;

#ifndef UNIT_H
#define UNIT_H

enum PathType{P_NORMAL, P_PASSUNITS, P_STAIRS, P_FLEE};

typedef struct {
    int len;
    short* pathXs; //-2 = stairs
    short* pathYs;
    bool cUnits;
} path;

typedef struct {
    short len;
    short type;
    Item* equips;
} MobEquips;

enum MobActionState{MAS_NONE, MAS_ATTACK, MAS_FLEE};

class Unit: public StatHolder {
    public:
        Unit(string n, StatHolder* prototype);
        virtual ~Unit();
        void move(int dir);
        short x;
        short y;
        int theTime;
        string name;

        path* currentPath;
        short pointOnPath;
        unsigned short actionState;
        MobEquips* equipment;

        void setStat(int stat, int value);
        short getStatValue(int stat);
        short modifyStat(int stat, int amount);
        void setStatF(int stat, float value);
        float getStatValueF(int stat);
        float modifyStat(int stat, float amount);

        void needToUpdate(int stat, bool isFloat);
        bool hasStat(int stat, bool isFloat);
        graphic getGraphic();
        void setEnemy(Unit* enemy);
        Unit* getEnemy();
    private:
        StatHolder* unitPrototype;
        Unit* currentEnemy;
        graphic g;
};

#endif // UNIT_H
