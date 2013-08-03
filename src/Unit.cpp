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

#include <iostream>
using namespace std;
#include "Unit.h"

Unit::Unit(string n, StatHolder* prototype): StatHolder(V_UNIT), name(n) {
    for (unsigned int i = 0; i < name.size(); ++i) {
        if (name[i] == '_') {
            name[i] = ' ';
        }
    }
    if (prototype) {
        graphic.loc = prototype->getStatValue(Stat::GLOC);
        int spe = prototype->getStatValue(Stat::GTYPE);
        if (spe) graphic.loc += rand() % spe;
        graphic.tex = Texture::get(prototype->getStatValue(Stat::GTEX));

        // TODO improve
        for (int i = 0; i < Stat::getNum(V_UNIT); i++) {
            Stat* stat = Stat::get(V_UNIT, i);
            int si = stat->getIndex();
            if (stat->getFormula()) {
                if (stat->isItFloat()) {
                    StatHolder::addStatVF(si, prototype->getStatValueF(si));
                } else {
                    StatHolder::addStatV(si, prototype->getStatValue(si));
                }
            }
        }

        unitPrototype = prototype;
        StatHolder::setStat(Stat::HP, prototype->getStatValue(Stat::MAXHP));
        StatHolder::setStat(Stat::MANA, prototype->getStatValue(Stat::MAXMANA));
    }
    pointOnPath = -1;
    currentPath = NULL;
    theTime = 0;
    equipment = NULL;
}

Unit::~Unit() {
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
        set<Stat*> enemyAfflictions = Stat::getEnemyAfflictions();
        for (set<Stat*>::iterator i = enemyAfflictions.begin(); i != enemyAfflictions.end(); ++i) {
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

const StatHolder* Unit::getProto() {
    return unitPrototype;
}

void Unit::save(ostream& saveData) {
    outStr(name, saveData);
    outPair(pos, saveData);
    outInt(theTime, saveData);

    if (equipment) {
        saveData.put('\1');
        outSht(equipment->type, saveData);
        outSht(equipment->len, saveData);
        for (int i = 0; i < equipment->len; i++) {
            equipment->equips[i].save(saveData);
        }
    } else saveData.put('\0');

    StatHolder::save(saveData);
    outSht(unitPrototype->getStatValue(Stat::INDEX), saveData);
}

Unit::Unit(std::istream& saveData): StatHolder(V_UNIT) {
    name = inStr(saveData);
    pos = inPair(saveData);
    theTime = inInt(saveData);

    bool isEquipment = saveData.get();
    if (isEquipment) {
        equipment = new MobEquips;
        equipment->type = inSht(saveData);
        equipment->len = inSht(saveData);
        equipment->equips = new Item[equipment->len];
        for (int i = 0; i < equipment->len; i++) {
            equipment->equips[i] = Item(saveData);
        }
    } else equipment = NULL;

    StatHolder::load(saveData);
    int n = inSht(saveData);
    unitPrototype = mobs[n - 1].proto;

    graphic.loc = unitPrototype->getStatValue(Stat::GLOC);
    int spe = unitPrototype->getStatValue(Stat::GTYPE);
    if (spe) graphic.loc += rand() % spe;
    graphic.tex = Texture::get(unitPrototype->getStatValue(Stat::GTEX));

    pointOnPath = -1;
    currentPath = NULL;
}

std::vector<Mob> Unit::mobs;
std::map<String, short> Unit::mobNameMap;
std::vector<Stat*> Unit::defaultStats;

void Unit::parseMob(YAML::Node fileNode, std::ostream& lerr) {
    String name = readYAMLStr(fileNode, "Name", "nil", "Unit lacks a name!", lerr);
    StatHolder* newUnit = new StatHolder(V_UNIT);
    for (YAML::Node::iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter) {
        String name = iter->first.as<String>();
        if (std::islower(name[0])) {
            if (Stat::has(name)) {
                YAML::Node val = iter->second;
                int statVal;
                if (val.IsSequence()) {
                    statVal = val[0].as<int>() + val[1].as<int>() * TEX_TILE_WIDTH;
                } else {
                    String s = val.as<String>();
                    if (isNum(s)) {
                        statVal = val.as<int>();
                    } else {
                        if (mobExists(s)) {
                            statVal =  mobNameMap[s];
                        } else if (ItemType::hasWeapType(s)) {
                            statVal = ItemType::getWeapType(s)->index;
                        } else {
                            statVal = Texture::get(s)->getIndex();
                        }
                    }
                }
                Stat* stat = Stat::get(name);
                if (stat->isItFloat()) {
                    newUnit->addStatVF(stat->getIndex(), statVal);
                } else {
                    newUnit->addStatV(stat->getIndex(), statVal);
                }
            } else {
                lerr << "'" + name + "' is not an existing stat name.\n";
            }
        }
    }

    for (unsigned int i = 0; i < defaultStats.size(); i++) {
        if (defaultStats[i]->isItFloat()) {
            newUnit->addStatF(defaultStats[i]->getIndex());
        } else {
            newUnit->addStat(defaultStats[i]->getIndex());
        }
    }
    mobNameMap[name] = mobs.size();
    mobs.push_back(Mob(name, newUnit));
    newUnit->addStatV(Stat::INDEX, mobs.size());
}

void Unit::parseDefaultStats(YAML::Node fileNode, std::ostream& lerr) {
    for (YAML::Node::iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter) {
        String statName = iter->as<String>();
        if (Stat::has(statName)) {
            defaultStats.push_back(Stat::get(iter->as<String>()));
        } else lerr << "'" << statName << "' is not an existing stat.\n";
    }
}

Mob* Unit::getMob(String name) {
    return &mobs[mobNameMap[name]];
}

Mob* Unit::getMob(short index) {
    return &mobs[index];
}

bool Unit::mobExists(String name) {
    return mobNameMap.find(name) != mobNameMap.end();
}
