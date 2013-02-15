#include <iostream>
using namespace std;
#include "Unit.h"

Unit::Unit(string n, StatHolder* prototype): StatHolder(V_UNIT) {
    x = 0;
    y = 0;
    name = n;
    for (unsigned int i = 0; i < name.size(); i++) {
        if (name[i] == '_') {
            name[i] = ' ';
        }
    }
    if (prototype) {
        g.loc = prototype->getStatValue(S_GLOC);
        int spe = prototype->getStatValue(S_GTYPE);
        if (spe) g.loc += rand() % spe;
        g.tex = prototype->getStatValue(S_GTEX);
        unsigned char* stats = prototype->getIntStats();
        for (int i = 0; i < prototype->getNumIntStats(); i++) {
            if (getStat(V_UNIT, stats[i])->getFormula()->getLength() > 1) {
                StatHolder::addStatV(stats[i], prototype->getStatValue(stats[i]));
            }
        }
        unsigned char* statsF = prototype->getFloatStats();
        for (int i = 0; i < prototype->getNumFloatStats(); i++) {
            if (getStat(V_UNIT, statsF[i])->getFormula()->getLength() > 1) {
                StatHolder::addStatVF(statsF[i], prototype->getStatValueF(stats[i]));
            }
        }
        unitPrototype = prototype;
        StatHolder::setStat(S_HP, prototype->getStatValue(S_MAXHP));
        StatHolder::setStat(S_MANA, prototype->getStatValue(S_MAXMANA));
    }
    g.border = 0;
    StatHolder::aThis = this;
    pointOnPath = -1;
    currentPath = NULL;
    theTime = 0;
    equipment = NULL;
}

Unit::~Unit() {
    //removeHashMaps();
    //unitPrototype->removeHashMaps();
    if (equipment) {
        delete[] equipment->equips;
        delete equipment;
    }
}

void Unit::setStat(int stat, int value) {
    if (StatHolder::hasStat(stat, false)) {
        StatHolder::setStat(stat, value);
    } else {
        StatHolder::addStatV(stat, value);
    }
}

void Unit::setStatF(int stat, float value) {
    if (StatHolder::hasStat(stat, true)) {
        StatHolder::setStatF(stat, value);
    } else {
        StatHolder::addStatVF(stat, value);
    }
}

short Unit::modifyStat(int stat, int amount) {
    if (StatHolder::hasStat(stat, false)) {
        return StatHolder::modifyStat(stat, amount);
    } else {
        int num = amount + unitPrototype->getStatValue(stat);
        StatHolder::addStatV(stat, num);
        StatHolder::statChanged(stat);
        return num;
    }
}

float Unit::modifyStatF(int stat, float amount) {
    if (StatHolder::hasStat(stat, true)) {
        return StatHolder::modifyStatF(stat, amount);
    } else {
        float num = amount + unitPrototype->getStatValueF(stat);
        StatHolder::addStatVF(stat, num);
        StatHolder::statChanged(stat);
        return num;
    }
}

short Unit::getStatValue(int stat) {
    if (StatHolder::hasStat(stat, false)) {
        return StatHolder::getStatValue(stat);
    } else {
        return unitPrototype->getStatValue(stat);
    }
}

float Unit::getStatValueF(int stat) {
    if (StatHolder::hasStat(stat, true)) {
        return StatHolder::getStatValueF(stat);
    } else {
        return unitPrototype->getStatValueF(stat);
    }
}

void Unit::needToUpdate(int stat, bool isFloat) {
    if (!StatHolder::hasStat(stat, isFloat)) {
        if (isFloat) {
            StatHolder::addStatVF(stat, unitPrototype->getStatValueF(stat));
        } else {
            StatHolder::addStatV(stat, unitPrototype->getStatValue(stat));
        }
    } else {
        StatHolder::needToUpdate(stat, isFloat);
    }
}

void Unit::setEnemy(Unit* enemy) {
    bool toUpdate = enemy != currentEnemy;
    if (toUpdate) {
        currentEnemy = enemy;
        set<Stat*> enemyAfflictions = getEnemyAfflictions();
        for (set<Stat*>::iterator i = enemyAfflictions.begin(); i != enemyAfflictions.end(); i++) {
            Stat* theStat = (Stat*)*i;
            StatHolder::statChanged(theStat->getIndex());
        }
    }
}

Unit* Unit::getEnemy() {
    return currentEnemy;
}

bool Unit::hasStat(int stat, bool isFloat) {
    return StatHolder::hasStat(stat, isFloat) || unitPrototype->hasStat(stat, isFloat);
}

StatHolder* Unit::getProto() {
    return unitPrototype;
}
