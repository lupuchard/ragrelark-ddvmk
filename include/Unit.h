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

#ifndef UNIT_H
#define UNIT_H

#include <string>
#include "StatHolder.h"
#include "Stat.h"
#include "Tile.h"
#include "Item.h"

enum PathType{P_NORMAL, P_PASSUNITS, P_STAIRS, P_FLEE};

typedef struct {
    int len;
    Coord* pathLocs; //x = -2 = stairs
    bool cUnits;
} Path;

typedef struct {
    short len;
    short type;
    Item* equips;
} MobEquips;

enum MobActionState{MAS_NONE, MAS_ATTACK, MAS_FLEE};

class Unit: public StatHolder {
    public:
        Unit(String n, StatHolder* prototype);
        virtual ~Unit();
        void move(int dir);
        Coord pos;
        int theTime;
        String name;
        Graphic graphic;

        Path* currentPath;
        short pointOnPath;
        unsigned short actionState;
        MobEquips* equipment;

        void setStat(int stat, int value);
        virtual short getStatValue(int stat);
        virtual short modifyStat(int stat, int amount);
        void setStatF(int stat, float value);
        virtual float getStatValueF(int stat);
        virtual float modifyStatF(int stat, float amount);

        void needToUpdate(int stat, bool isFloat);
        bool hasStat(int stat, bool isFloat);
        void setEnemy(Unit* enemy);
        Unit* getEnemy();

        StatHolder* getProto();
    private:
        StatHolder* unitPrototype;
        Unit* currentEnemy;
};

#endif // UNIT_H
