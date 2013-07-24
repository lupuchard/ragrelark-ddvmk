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

#include "StatHolder.h"

#define NUM_STAT_HOLDERS_SOFAR 3

#include <stdexcept>
#include <exception>

StatHolder::StatHolder(VOwner o) {
    owner = o;
    numIntStats = 0;
    numFloatStats = 0;
}

StatHolder::~StatHolder() { }

void StatHolder::addStat(int stat) {
    addStatV(stat, 0);
}

void StatHolder::addStatF(int stat) {
    addStatVF(stat, 0);
}

void StatHolder::addStatV(int stat, int value) {
    numIntStats++;
    intStats[stat] = std::pair<short, bool>(value, true);
}

void StatHolder::addStatVF(int stat, float value) {
    numFloatStats++;
    floatStats[stat] = std::pair<float, bool>(value, true);
}

void StatHolder::needToUpdate(int stat, bool isFloat) {
    if (Stat::get(owner, stat)->getFormula()) {
        if (isFloat) {
            floatStats[stat].second = true;
        } else {
            intStats[stat].second = true;
        }
        statChanged(stat);
    }
}

short StatHolder::getStatValue(int stat) {
    std::unordered_map<unsigned char, std::pair<short, bool> >::iterator i = intStats.find(stat);
    if (i == intStats.end()) return 0;
    std::pair<short, bool> val = i->second;
    if (val.second) {
        Stat* s = Stat::get((VOwner)owner, stat);
        int temp = val.first;
        if (s->getFormula()) val.first = s->getFormula()->run(Stat::getFormUser(), this, val.first);
        if (temp != val.first) {
            Stat::getFormUser()->statChanged(stat, this);
        }
        val.second = false;
        intStats[stat] = val;
    }
    return val.first;
}

float StatHolder::getStatValueF(int stat) {
    std::unordered_map<unsigned char, std::pair<float, bool> >::iterator i = floatStats.find(stat);
    if (i == floatStats.end()) return 0;
    std::pair<float, bool> val = i->second;
    if (val.second) {
        Stat* s = Stat::get((VOwner)owner, stat);
        float temp = val.first;
        if (s->getFormula()) val.first = s->getFormula()->runFloat(Stat::getFormUser(), this, val.first);
        if (temp != val.first) {
            Stat::getFormUser()->statChanged(stat, this);
        }
        val.second = false;
        floatStats[stat] = val;
    }
    return val.first;
}

VOwner StatHolder::getOwner() {
    return (VOwner)owner;
}

void StatHolder::setStat(int stat, int value) {
    intStats[stat] = std::pair<short, bool>(value, true);
    Stat::getFormUser()->statChanged(stat, this);
}

void StatHolder::setStatF(int stat, float value) {
    floatStats[stat] = std::pair<float, bool>(value, true);
    Stat::getFormUser()->statChanged(stat, this);
}

void StatHolder::statChanged(int stat) {
    Stat::getFormUser()->statChanged(stat, this);
}

short StatHolder::modifyStat(int stat, int amount) {
    std::pair<short, bool> v = intStats[stat];
    v.first += amount;
    v.second = true;
    intStats[stat] = v;
    Stat::getFormUser()->statChanged(stat, this);
    return v.first;
}

float StatHolder::modifyStatF(int stat, float amount) {
    std::pair<float, bool> v = floatStats[stat];
    v.first += amount;
    v.second = true;
    floatStats[stat] = v;
    Stat::getFormUser()->statChanged(stat, this);
    return v.first;
}

bool StatHolder::hasStat(int stat, bool isFloat) {
    if (isFloat) {
        return floatStats.count(stat);
    }
    return intStats.count(stat);
}

int StatHolder::getNumStats() {
    return numIntStats + numFloatStats;
}

int StatHolder::getNumIntStats() {
    return numIntStats;
}

int StatHolder::getNumFloatStats() {
    return numFloatStats;
}

void StatHolder::print() {
    for (std::unordered_map<unsigned char, std::pair<short, bool> >::iterator iter = intStats.begin(); iter != intStats.end(); ++iter) {
        std::cout << Stat::get(owner, iter->first)->getName() << ": " << iter->second.first << "\n";
    }
    std::cout.flush();
}
