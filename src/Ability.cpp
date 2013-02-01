#include "Ability.h"
#include <map>

map<int, Ability*> abilities;
void addAbility(Ability* ability) {
    abilities[ability->getI()] = ability;
}
Ability* getAbility(int index) {
    map<int, Ability*>::iterator i = abilities.find(index);
    if (i == abilities.end()) return NULL;
    return i->second;
}
Ability* getAbility(bool wind, bool earth, bool water, bool fire, bool phys, bool ment, bool light, bool dark, int power) {
    return abilities[(wind << 7) | (earth << 1) | (water << 3) | (fire << 9) | (phys << 4) | (ment << 6) | (light << 8) | (dark << 2) | power];
}
void cleanAbilities() {
    for (map<int, Ability*>::iterator i = abilities.begin(); i != abilities.end(); i++) {
        delete i->second;
    }
    abilities.clear();
}

Ability::Ability(string n, int mCost, int d, int tt, int i) {
    name = n;
    manaCost = mCost;
    difficulty = d;
    timeTake = tt;
    classificationI = i;
}

string Ability::getName() {
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
