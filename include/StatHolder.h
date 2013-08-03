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

#ifndef STATHOLDER_H
#define STATHOLDER_H

#include "Stat.h"

#define MAX_HUNGER 5000
#define MAX_STAMINA 10000

/* A StatHolder has and a manages a bunch of stats and values for them. */
/* This class was constructed with only memory in mind, so it's not necessarily as fast as it could be. */
class StatHolder: public StatHolderIntef {
    public:
        StatHolder(VOwner owner);
        virtual ~StatHolder();

        /* Adds a stat to the StatHolder (with or without a value). addStatF and addStatVF add float stats. */
        void addStat(int statI);
        void addStatV(int statI, int value);
        void addStatF(int statI);
        void addStatVF(int statI, float value);

        /* Sets or modifies the value of a specific stat. */
        virtual void setStat(int stat, int value);
        virtual short modifyStat(int stat, int amount);
        virtual void setStatF(int stat, float value);
        virtual float modifyStatF(int stat, float amount);

        /* Returns the value stored for a specific stat. If a stat is not stored, it returns 0. */
        virtual short getStatValue(int stat);
        virtual float getStatValueF(int stat);

        /* Informs the StatHolder that a stat has changed. Should not need to be called outside this class because of the stat setting methods call this. */
        void statChanged(int stat);

        /* Informs the StatHolder that a stat should recalculate itself (probably because a variable in the formula has changed. */
        virtual void needToUpdate(int stat, bool isFloat);

        VOwner getOwner();

        /* Returns true if this StatHolder stores a value for this stat. */
        virtual bool hasStat(int stat, bool isFloat);

        int getNumStats();
        int getNumIntStats();
        int getNumFloatStats();

        void print();

        void save(std::ostream& saveData);
        void load(std::istream& saveData);
    private:
        unsigned int numIntStats;
        unsigned int numFloatStats;
        VOwner owner;

        //std::hash_map<stat index, std::pair<value, to be updated> >
        std::unordered_map<unsigned char, std::pair<short, bool> > intStats;
        std::unordered_map<unsigned char, std::pair<float, bool> > floatStats;
};

#endif // STATHOLDER_H
