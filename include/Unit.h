#include <string>
#include "StatHolder.h"
#include "Stat.h"
#include "Tile.h"
using namespace std;

#ifndef UNIT_H
#define UNIT_H

#define P_NORMAL 0
#define P_PASSUNITS 1
#define P_STAIRS 2

typedef struct {
    int len;
    short* pathXs; //-2 = stairs
    short* pathYs;
    bool cUnits;
} path;

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
