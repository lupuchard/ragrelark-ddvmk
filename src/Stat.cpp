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

#include "Stat.h"

std::vector<Stat*> unitStats;
std::vector<Stat*> worldStats;
std::vector<Stat*> itemStats;

Stat* getStat(VOwner type, int statI) {
    switch(type) {
        case V_UNIT: return unitStats[statI]; break;
        case V_WORLD: return worldStats[statI]; break;
        case V_ITEM: return itemStats[statI]; break;
        default: return unitStats[statI]; break;
    }
}
int addStat(VOwner type, Stat* stat) {
    switch(type) {
        case V_UNIT: unitStats.push_back(stat); return unitStats.size() - 1; break;
        case V_WORLD: worldStats.push_back(stat); return worldStats.size() - 1; break;
        case V_ITEM: itemStats.push_back(stat); return itemStats.size() - 1; break;
        default: unitStats.push_back(stat); return unitStats.size() - 1; break;
    }
}
int getNumStats(VOwner type) {
    switch(type) {
        case V_UNIT: return unitStats.size(); break;
        case V_WORLD: return worldStats.size(); break;
        case V_ITEM: return itemStats.size(); break;
        default: return unitStats.size(); break;
    }
}

Stat::Stat(std::string n, Formula* f, unsigned char ind, bool fl): name(n) {
    formula = f;
    isFloat = fl;
    index = ind;
}

Stat::~Stat() {
    //dtor
}

std::string Stat::getName() {
    return name;
}

Formula* Stat::getFormula() {
    return formula;
}

bool Stat::isItFloat() {
    return isFloat;
}

unsigned char Stat::getIndex() {
    return index;
}
