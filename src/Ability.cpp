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

Ability::Ability(String n, int mCost, int d, int tt, int i): name(n) {
    manaCost = mCost;
    difficulty = d;
    timeTake = tt;
    classificationI = i;
}

String Ability::getName() {
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

std::map<int, Ability*> Ability::abilities;
std::map<String, Ability*> Ability::abilityNameMap;
std::map<String, int> Ability::elemNameMap;

void Ability::parseElem(YAML::Node fileNode) {
    int i = 0;
    for (YAML::Node::iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter, i++) {
        elemNameMap[iter->as<String>()] = i;
    }
}
void Ability::parse(YAML::Node fileNode) {
    String name = readYAMLStr(fileNode, "Name", "nil", "Ability lacks a name!");
    int manaCost = readYAMLInt(fileNode, "Cost", 1, "Ability last a mana cost!");
    int difficulty = readYAMLInt(fileNode, "Diff", 10, "Ability lacks a difficulty!");
    int timeTake = readYAMLInt(fileNode, "Delay", 20, "Ability lacks a delay!");

    int index = 0;
    YAML::Node trigger = fileNode["Trigger"];
    for (YAML::Node::iterator iter = trigger.begin(); iter != trigger.end(); ++iter) {
        index |= 1 << elemNameMap[iter->as<String>()];
    }

    Ability* newAbility = new Ability(name, manaCost, difficulty, timeTake, index);
    abilities[index] = newAbility;
    abilityNameMap[name] = newAbility;
}

Ability* Ability::get(int index) {
    std::map<int, Ability*>::iterator i = abilities.find(index);
    if (i == abilities.end()) return NULL;
    return i->second;
}

Ability* Ability::get(bool wind, bool earth, bool water, bool fire, bool phys, bool ment, bool light, bool dark, int power) {
    return abilities[(wind << 1) | (earth << 2) | (water << 3) | (fire << 4) | (phys << 5) | (ment << 6) | (light << 7) | (dark << 8) | power];
}

Ability* Ability::get(String name) {
    return abilityNameMap[name];
}

bool Ability::has(String name) {
    return abilityNameMap.find(name) != abilityNameMap.end();
}

void Ability::clean() {
    for (std::map<int, Ability*>::iterator i = abilities.begin(); i != abilities.end(); ++i) {
        delete i->second;
    }
    abilities.clear();
}
