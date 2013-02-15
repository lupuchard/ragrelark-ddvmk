#include "Swarmer.h"

set<unsigned short> direct;
set<unsigned short> inverse;

Swarmer::Swarmer(string n, StatHolder* prototype): Unit(n, prototype) { }
Swarmer::~Swarmer() {
    for (unsigned int i = 0; i < others.size(); i++) {
        delete others[i];
    }
}

short Swarmer::getStatValue(int stat) {
    if (stat == S_HP) {
        int h = Unit::getStatValue(S_HP);
        for (unsigned int i = 0; i < others.size(); i++) {
            h += others[i]->getStatValue(S_HP);
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
    if (stat == S_HP) {
        if (others.empty()) {
            int rem = Unit::modifyStat(S_HP, amount);
            return rem;
        } else {
            int target = rand() % others.size();
            int rem = others[target]->modifyStat(S_HP, amount);
            if (rem > 0) {
                return rem;
            } else {
                delete others[target];
                others.erase(others.begin() + target);
                Unit::g.loc = Unit::getStatValue(S_GLOC) + others.size();
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
    Unit::g.loc = Unit::getStatValue(S_GLOC) + others.size();
}

Unit* Swarmer::take(int unit) {
    Unit* temp = others[unit];
    others.erase(others.begin() + unit);
    Unit::g.loc = Unit::getStatValue(S_GLOC) + others.size();
    return temp;
}
