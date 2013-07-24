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

enum PathType{PATH_NORMAL, PATH_PASSUNITS, PATH_STAIRS, PATH_FLEE};
struct Path {
    int len;
    Coord* pathLocs; //x = -2 = stairs
    bool cUnits;
};

struct MobEquips {
    short len;
    short type;
    Item* equips;
};

enum MobActionState{MAS_NONE, MAS_ATTACK, MAS_FLEE};

class Unit: public StatHolder {
    public:
        Unit(String n, StatHolder* prototype);
        virtual ~Unit();

        /// The position of the unit in it's current zone.
        Coord pos;

        /// The time the world was at when this unit finished it's last action.
        int theTime;

        /// The name of the unit.
        String name;

        /// The graphic of the unit. Exists in the Prototype but is also stored here for efficiency.
        Graphic graphic;

        /// The current path that the unit is taking, created by the pather.
        Path* currentPath;
        short pointOnPath;

        /// The equipment the unit is holding, if any.
        MobEquips* equipment;

        void setStat(int stat, int value);
        virtual short getStatValue(int stat);
        virtual short modifyStat(int stat, int amount);
        void setStatF(int stat, float value);
        virtual float getStatValueF(int stat);
        virtual float modifyStatF(int stat, float amount);
        void needToUpdate(int stat, bool isFloat);
        bool hasStat(int stat, bool isFloat);

        /// Sets the unit's current enemy. Set right before attacking to calculate damage stats.
        void setEnemy(Unit* enemy);
        Unit* getEnemy();

        /// The Units prototype, another StatHolder that holds stats shared among units of the same type.
        const StatHolder* getProto();
    private:
        StatHolder* unitPrototype;
        Unit* currentEnemy;
};

#endif // UNIT_H
