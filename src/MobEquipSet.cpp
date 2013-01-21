#include "MobEquipSet.h"

MobEquipSet::MobEquipSet() {
    allEquips = vector<pair<int, vector<int> > >();
}

int MobEquipSet::addEquips(int weight) {
    allEquips.push_back(pair<int, vector<int> >(weight, vector<int>()));
    return allEquips.size() - 1;
}
void MobEquipSet::addEquipToEquips(int equips, int equip) {
    allEquips[equips].second.push_back(equip);
}
int MobEquipSet::getEquips(unsigned int index, int* equips) {
    for (unsigned int i = 0; i < allEquips[index].second.size(); i++) {
        equips[i] = allEquips[index].second[i];
    }
    return allEquips[index].second.size();
}
int MobEquipSet::getRandEquips(int* equips) {
    int index = getRandIndex();
    for (unsigned int j = 0; j < allEquips[index].second.size(); j++) {
        equips[j] = allEquips[index].second[j];
    }
    return allEquips[index].second.size();
}
int MobEquipSet::getRandEquipsNear(unsigned int index, int* equips) {
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
        equips[j] = allEquips[i].second[j];
    }
    return allEquips[i].second.size();
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
