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

#include "MobEquipSet.h"

MobEquipSet::MobEquipSet() { }

int MobEquipSet::addEquips(int weight) {
    allEquips.push_back(std::make_pair(weight, std::vector<ItemType*>()));
    return allEquips.size() - 1;
}
void MobEquipSet::addEquipToEquips(int equips, ItemType* equip) {
    allEquips[equips].second.push_back(equip);
}
void MobEquipSet::getEquips(unsigned int index, std::vector<ItemType*>& equips) {
    for (unsigned int i = 0; i < allEquips[index].second.size(); i++) {
        equips.push_back(allEquips[index].second[i]);
    }
}
void MobEquipSet::getRandEquips(std::vector<ItemType*>& equips) {
    int index = getRandIndex();
    for (unsigned int j = 0; j < allEquips[index].second.size(); j++) {
        equips.push_back(allEquips[index].second[j]);
    }
}
void MobEquipSet::getRandEquipsNear(unsigned int index, std::vector<ItemType*>& equips) {
    int totalWeight = 0;
    totalWeight += allEquips[index].first * 10;
    if (index > 0) totalWeight += allEquips[index - 1].first;
    if (index + 1 < allEquips.size()) totalWeight += allEquips[index + 1].first;
    int r = rand() % totalWeight;
    int currWeight = 0;
    int i = index;
    currWeight += allEquips[index].first * 10;
    if (r < currWeight) i = index;
    else if (index > 0 && r < currWeight + allEquips[index - 1].first) i = index - 1;
    else i = index + 1;
    for (unsigned int j = 0; j < allEquips[i].second.size(); j++) {
        equips.push_back(allEquips[i].second[j]);
    }
}
int MobEquipSet::getNumEquipTypes() {
    return allEquips.size();
}
int MobEquipSet::getRandIndex() {
    int totalWeight = 0;
    for (unsigned int i = 0; i < allEquips.size(); i++) {
        totalWeight += allEquips[i].first;
    }
    int r = rand() % totalWeight;
    int currWeight = 0;
    for (unsigned int i = 0; i < allEquips.size(); i++) {
        currWeight += allEquips[i].first;
        if (r < currWeight) {
            return i;
        }
    }
    return 0; //should not happen
}

std::map<String, MobEquipSet*> MobEquipSet::mobEquipSetNameMap;

void MobEquipSet::parseAll(YAML::Node fileNode) {
    for (YAML::const_iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter) {
        String name = iter->first.as<String>();
        YAML::Node setNode = iter->second;
        MobEquipSet* newMobEquipsSet = new MobEquipSet();
        for (YAML::const_iterator jter = setNode.begin(); jter != setNode.end(); ++jter) {
            int ei = newMobEquipsSet->addEquips(1);
            for (YAML::const_iterator kter = jter->begin(); kter != jter->end(); ++kter) {
                String name = kter->as<String>();
                if (ItemType::has(name)) {
                    newMobEquipsSet->addEquipToEquips(ei, ItemType::get(name));
                } else {
                    std::cout << "'" << name << "' is not an existing item.\n";
                }
            }
        }
        mobEquipSetNameMap[name] = newMobEquipsSet;
    }
}
MobEquipSet* MobEquipSet::get(String name) {
    return mobEquipSetNameMap[name];
}
bool MobEquipSet::has(String name) {
    return mobEquipSetNameMap.find(name) != mobEquipSetNameMap.end();
}
void MobEquipSet::clean() {
    for (std::map<String, MobEquipSet*>::iterator iter = mobEquipSetNameMap.begin(); iter != mobEquipSetNameMap.end(); ++iter) {
        delete iter->second;
    }
    mobEquipSetNameMap.clear();
}
