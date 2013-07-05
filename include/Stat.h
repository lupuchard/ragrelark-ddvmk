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

#include "Formula.h"
#include <string>

#ifndef STAT_H
#define STAT_H

/* This defines a type of statistic. */
class Stat {
    public:
        /* No two stats of the same isFloat status can have the same index. */
        Stat(std::string name, Formula* formula, unsigned char index, bool isFloat);
        virtual ~Stat();

        /* This is mostly used for parsing, but may later be used in-game. */
        std::string getName();

        /* The formula that defines the statistic. A formula with a length of 1 (assumed to be either SLF or some constant) will never be recalled. */
        Formula* getFormula();

        bool isItFloat();
        unsigned char getIndex();
    protected:
    private:
        Formula* formula;
        std::string name;
        bool isFloat;
        unsigned char index;
};

Stat* getStat(VOwner type, int statI);
int getNumStats(VOwner type);
int addStat(VOwner type, Stat* theStat);

#endif // STAT_H
