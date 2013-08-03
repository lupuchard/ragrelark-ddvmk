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

#include "Swarmer.h"

std::set<unsigned short> direct;
std::set<unsigned short> inverse;

Swarmer::Swarmer(String n, StatHolder* prototype): Unit(n, prototype) { }
Swarmer::~Swarmer() {
    for (unsigned int i = 0; i < others.size(); i++) {
        delete others[i];
    }
}

short Swarmer::getStatValue(int stat) {
    if (stat == Stat::HP) {
        int h = Unit::getStatValue(Stat::HP);
        for (unsigned int i = 0; i < others.size(); i++) {
            h += others[i]->getStatValue(Stat::HP);
        }
        return h;
    } else if (direct.find(stat) != direct.end()) {
        return Unit::getStatValue(stat) * howMany();
    } else if (inverse.find(stat) != inverse.end()) {
        return Unit::getStatValue(stat) / howMany();
    } else {
        return Unit::getStatValue(stat);
    }
}

short Swarmer::modifyStat(int stat, int amount) {
    if (stat == Stat::HP) {
        if (others.empty()) {
            int rem = Unit::modifyStat(Stat::HP, amount);
            return rem;
        } else {
            int target = rand() % others.size();
            int rem = others[target]->modifyStat(Stat::HP, amount);
            if (rem > 0) {
                return rem;
            } else {
                delete others[target];
                others.erase(others.begin() + target);
                Unit::graphic.loc = Unit::getStatValue(Stat::GLOC) + others.size();
                return 1;
            }
        }
    } else {
        return Unit::modifyStat(stat, amount);
    }
}

float Swarmer::getStatValueF(int stat) {
    if (direct.find(stat) != direct.end()) {
        return Unit::getStatValueF(stat) * howMany();
    } else if (inverse.find(stat) != direct.end()) {
        return Unit::getStatValueF(stat) / howMany();
    } else {
        return Unit::getStatValueF(stat);
    }
}

float Swarmer::modifyStatF(int stat, float amount) {
    return Unit::modifyStatF(stat, amount);
}

int Swarmer::howMany() {
    return others.size() + 1;
}

void Swarmer::add(Swarmer* unit) {
    while (unit->howMany() > 1) {
        others.push_back(unit->others[0]);
        unit->take(0);
    }
    others.push_back(unit);
    Unit::graphic.loc = Unit::getStatValue(Stat::GLOC) + others.size();
}

Unit* Swarmer::take(int unit) {
    Unit* temp = others[unit];
    others.erase(others.begin() + unit);
    Unit::graphic.loc = Unit::getStatValue(Stat::GLOC) + others.size();
    return temp;
}

void Swarmer::save(std::ostream& saveData) {
    Unit::save(saveData);
    outSht(others.size(), saveData);
    for (unsigned int i = 0; i < others.size(); i++) {
        others[i]->save(saveData);
    }
}

Swarmer::Swarmer(std::istream& saveData): Unit(saveData) {
    unsigned int numOthers = inSht(saveData);
    for (unsigned int i = 0; i < numOthers; i++) {
        others.push_back(new Swarmer(saveData));
    }
}
