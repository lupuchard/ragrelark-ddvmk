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

#include "Ability.h"
#include <map>

std::map<int, Ability*> abilities;
void addAbility(Ability* ability) {
    abilities[ability->getI()] = ability;
}
Ability* getAbility(int index) {
    std::map<int, Ability*>::iterator i = abilities.find(index);
    if (i == abilities.end()) return NULL;
    return i->second;
}
Ability* getAbility(bool wind, bool earth, bool water, bool fire, bool phys, bool ment, bool light, bool dark, int power) {
    return abilities[(wind << 7) | (earth << 1) | (water << 3) | (fire << 9) | (phys << 4) | (ment << 6) | (light << 8) | (dark << 2) | power];
}
void cleanAbilities() {
    for (std::map<int, Ability*>::iterator i = abilities.begin(); i != abilities.end(); ++i) {
        delete i->second;
    }
    abilities.clear();
}

Ability::Ability(std::string n, int mCost, int d, int tt, int i): name(n) {
    manaCost = mCost;
    difficulty = d;
    timeTake = tt;
    classificationI = i;
}

std::string Ability::getName() {
    return name;
}

int Ability::getCost() {
    return manaCost;
}

int Ability::getDifficulty() {
    return difficulty;
}

int Ability::getI() {
    return classificationI;
}

int Ability::getTimeTake() {
    return timeTake;
}
