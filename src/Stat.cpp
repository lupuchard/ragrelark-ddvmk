#include "Stat.h"

vector<Stat*> unitStats;
vector<Stat*> worldStats;
vector<Stat*> itemStats;

Stat* getStat(VOwner type, int statI) {
    switch(type) {
        case V_UNIT: return unitStats[statI]; break;
        case V_WORLD: return worldStats[statI]; break;
        case V_ITEM: return itemStats[statI]; break;
        default: return unitStats[statI]; break;
    }
}
int addStat(VOwner type, Stat* stat) {
    switch(type) {
        case V_UNIT: unitStats.push_back(stat); return unitStats.size() - 1; break;
        case V_WORLD: worldStats.push_back(stat); return worldStats.size() - 1; break;
        case V_ITEM: itemStats.push_back(stat); return itemStats.size() - 1; break;
        default: unitStats.push_back(stat); return unitStats.size() - 1; break;
    }
}
int getNumStats(VOwner type) {
    switch(type) {
        case V_UNIT: return unitStats.size(); break;
        case V_WORLD: return worldStats.size(); break;
        case V_ITEM: return itemStats.size(); break;
        default: return unitStats.size(); break;
    }
}

Stat::Stat(string n, Formula* f, unsigned char ind, bool fl) {
    name = n;
    formula = f;
    isFloat = fl;
    index = ind;
}

Stat::~Stat() {
    //dtor
}

string Stat::getName() {
    return name;
}

Formula* Stat::getFormula() {
    return formula;
}

bool Stat::isItFloat() {
    return isFloat;
}

unsigned char Stat::getIndex() {
    return index;
}
