#include "StatHolder.h"

#define NUM_STAT_HOLDERS_SOFAR 3

FormulaUser* formUser;
void setFormUser(FormulaUser* fUser) {
    formUser = fUser;
}

map<completeStat, vector<completeStat>*> afflictions;
map<int, vector<completeStat>*> conAfflictions;
vector<Stat*> itemAfflictions;
vector<Stat*> enemyAfflictions;
//vector<int> itemConAfflictions;
//vector<int> enemyConAfflictions;
vector<Stat*> getItemAfflictions() {
    return itemAfflictions;
}
vector<Stat*> getEnemyAfflictions() {
    return enemyAfflictions;
}
vector<completeStat>* getAfflictions(Stat* s, int ownertype) {
    completeStat cStat = pair<int, Stat*>(ownertype, s);
    map<completeStat, vector<completeStat>*>::iterator p = afflictions.find(cStat);
    if (p != afflictions.end()) {
        return p->second;
    }
    return NULL;
}
vector<completeStat>* getConAfflictions(int condition){
    map<int, vector<completeStat>*>::iterator p = conAfflictions.find(condition);
    if (p != conAfflictions.end()) {
        return p->second;
    }
    return NULL;
}

void addAffliction(completeStat afflictingStat, Stat* afflictedStat, int edOwner) {
    if (edOwner == V_UNIT) {
        switch(afflictingStat.first) {
            case V_ITEM: itemAfflictions.push_back(afflictedStat); break;
            case V_ENEMY: enemyAfflictions.push_back(afflictedStat); break;
            default: {
                if (afflictions.find(afflictingStat) == afflictions.end()) {
                    afflictions[afflictingStat] = new vector<completeStat>;
                }
                afflictions[afflictingStat]->push_back(completeStat(edOwner, afflictedStat));
            } break;
        }
    } else {
        if (afflictions.find(afflictingStat) == afflictions.end()) {
            afflictions[afflictingStat] = new vector<completeStat>;
        }
        afflictions[afflictingStat]->push_back(completeStat(edOwner, afflictedStat));
    }
}

void addConAffliction(int afflictingCondition, int afflictingConOwner, Stat* afflictedStat, int edOwner) {
    if (edOwner == V_UNIT) {
        switch(afflictingConOwner) {
            case V_ENEMY: enemyAfflictions.push_back(afflictedStat); break;
            default: {
                if (conAfflictions.find(afflictingCondition) == conAfflictions.end()) {
            conAfflictions[afflictingCondition] = new vector<completeStat>;
        }
        conAfflictions[afflictingCondition]->push_back(completeStat(edOwner, afflictedStat));
            }
        }
    } else {
        if (conAfflictions.find(afflictingCondition) == conAfflictions.end()) {
            conAfflictions[afflictingCondition] = new vector<completeStat>;
        }
        conAfflictions[afflictingCondition]->push_back(completeStat(edOwner, afflictedStat));
    }
}

/*vector<int> getItemConAfflictions() {
    return itemConAfflictions;
}
vector<int> getEnemyConAfflictions() {
    return enemyConAfflictions;
}
completeCon* getConAfflictions(int condition, int ownerType) {
    //completeStat cStat = pair<int, Stat*>(ownertype, s);
    map<completeCon, vector<completeCon>*>::iterator p = afflictions.find(cStat);
    if (p != afflictions.end()) {
        return p->second;
    }
    return NULL;
}
void addConAffliction(vector<int, int> afflictingCon, int afflictedCon, int edOwner);*/

StatHolder::StatHolder(VOwner o) {
    owner = o;
    numIntStats = 0;
    numFloatStats = 0;
    intStats = new unsigned char[0];
    intValues = new short[0];
    floatStats = new unsigned char[0];
    floatValues = new float[0];
    aThis = this;
}

StatHolder::~StatHolder() {
    delete[] intStats;
    delete[] intValues;
    delete[] floatStats;
    delete[] floatValues;
}

void StatHolder::addStat(int stat) {
    addStatV(stat, 0);
}

void StatHolder::addStatF(int stat) {
    addStatVF(stat, 0);
}

void StatHolder::addStatV(int stat, int value) {
    unsigned char* newStats = new unsigned char[numIntStats + 1];
    short* newValues = new short[numIntStats + 1];
    int i;
    for (i = 0; i < numIntStats; i++) {
        if (stat < intStats[i]) {
            newStats[i] = stat;
            newValues[i] = value;
            break;
        } else {
            newStats[i] = intStats[i];
            newValues[i] = intValues[i];
        }
    }
    if (i == numIntStats) {
        newStats[i] = stat;
        newValues[i] = value;
    } else {
        for (int k = numIntStats - 1; k >= i; k--) {
            newStats[k + 1] = intStats[k];
            newValues[k + 1] = intValues[k];
            toBeUpdatedInt[k + 1] = toBeUpdatedInt[k];
        }
    }
    numIntStats++;
    delete[] intStats;
    intStats = newStats;
    delete[] intValues;
    intValues = newValues;
    toBeUpdatedInt[i] = true;
}

void StatHolder::addStatVF(int stat, float value) {
    unsigned char* newStats = new unsigned char[numFloatStats + 1];
    float* newValues = new float[numFloatStats + 1];
    int i;
    for (i = 0; i < numFloatStats; i++) {
        if (stat < floatStats[i]) {
            newStats[i] = stat;
            newValues[i] = value;
            break;
        } else {
            newStats[i] = floatStats[i];
            newValues[i] = floatValues[i];
        }
    }
    if (i == numFloatStats) {
        newStats[i] = stat;
        newValues[i] = value;
    } else {
        for (int k = numFloatStats - 1; k >= i; k--) {
            newStats[k + 1] = floatStats[k];
            newValues[k + 1] = floatValues[k];
            toBeUpdatedFloat[k + 1] = toBeUpdatedFloat[k];
        }
    }
    numFloatStats++;
    delete[] floatStats;
    floatStats = newStats;
    delete[] floatValues;
    floatValues = newValues;
    toBeUpdatedFloat[i] = true;
}

void StatHolder::needToUpdate(int stat, bool isFloat) {
    if (isFloat) {
        int i = binarySearchFloat(0, numFloatStats - 1, stat);
        toBeUpdatedFloat[i] = true;
    } else {
        int i = binarySearchInt(0, numIntStats - 1, stat);
        toBeUpdatedInt[i] = true;
    }
}

short StatHolder::getStatValue(int stat) {
    int i = binarySearchInt(0, numIntStats - 1, stat);
    if (i == -1) {
        return 0;
    }
    if (toBeUpdatedInt[i]) {
        Stat* s = getStat((VOwner)owner, stat);
        int temp = intValues[i];
        intValues[i] = s->getFormula()->run(formUser, aThis, intValues[i]);
        if (temp != intValues[i]) {
            formUser->statChanged(stat, aThis);
        }
        toBeUpdatedInt[i] = false;
    }
    return intValues[i];
}

float StatHolder::getStatValueF(int stat) {
    int i = binarySearchFloat(0, numFloatStats - 1, stat);
    if (i == -1) {
        return 0;
    }
    if (toBeUpdatedFloat[i]) {
        Stat* s = getStat((VOwner)owner, stat);
        float temp = floatValues[i];
        floatValues[i] = s->getFormula()->runFloat(formUser, aThis, floatValues[i]);
        if (temp != floatValues[i]) {
            formUser->statChanged(stat, aThis);
        }
        toBeUpdatedFloat[i] = false;
    }
    return floatValues[i];
}

int StatHolder::binarySearchInt(int first, int last, int stat) {
    while(first <= last) {
        int mid = (first + last) / 2;
        if (stat > intStats[mid]) {
            first = mid + 1;
        } else if (stat < intStats[mid]) {
            last = mid - 1;
        } else {
            return mid;
        }
    }
    return -1;
}

int StatHolder::binarySearchFloat(int first, int last, int stat) {
    while(first <= last) {
        int mid = (first + last) / 2;
        if (stat > floatStats[mid]) {
            first = mid + 1;
        } else if (stat < floatStats[mid]) {
            last = mid - 1;
        } else {
            return mid;
        }
    }
    return -1;
}

VOwner StatHolder::getOwner() {
    return (VOwner)owner;
}

void StatHolder::setStat(int stat, int value) {
    int i = binarySearchInt(0, numIntStats - 1, stat);
    if (i != -1) {
        intValues[i] = value;
        formUser->statChanged(stat, aThis);
        toBeUpdatedInt[i] = true;
    }
}

void StatHolder::setStatF(int stat, float value) {
    int i = binarySearchFloat(0, numFloatStats - 1, stat);
    if (i != -1) {
        floatValues[i] = value;
        formUser->statChanged(stat, aThis);
        toBeUpdatedFloat[i] = true;
    }
}

void StatHolder::statChanged(int stat) {
    formUser->statChanged(stat, aThis);
}

short StatHolder::modifyStat(int stat, int amount) {
    int i = binarySearchInt(0, numIntStats - 1, stat);
    if (i != -1) {
        intValues[i] = intValues[i] + amount;
        formUser->statChanged(stat, aThis);
        toBeUpdatedInt[i] = true;
        return intValues[i];
    }
    return 0;
}

float StatHolder::modifyStatF(int stat, float amount) {
    int i = binarySearchFloat(0, numFloatStats - 1, stat);
    if (i != -1) {
        floatValues[i] = floatValues[i] + amount;
        formUser->statChanged(stat, aThis);
        toBeUpdatedFloat[i] = true;
        return floatValues[i];
    }
    return 0;
}

bool StatHolder::hasStat(int stat, bool isFloat) {
    if (isFloat) {
        return (binarySearchFloat(0, numFloatStats - 1, stat) != -1);
    }
    return (binarySearchInt(0, numIntStats - 1, stat) != -1);
}

int StatHolder::getNumStats() {
    return numIntStats + numFloatStats;
}

int StatHolder::getNumIntStats() {
    return numIntStats;
}

int StatHolder::getNumFloatStats() {
    return numFloatStats;
}

unsigned char* StatHolder::getIntStats() {
    return intStats;
}

unsigned char* StatHolder::getFloatStats() {
    return floatStats;
}

bool StatHolder::getCondition(int conditionI) {
    return conditions[conditionI];
}

void StatHolder::setCondition(int conditionI, bool value) {
    conditions[conditionI] = value;
    cout << "step 1" << endl;
    formUser->conditionChanged(conditionI, aThis);
}

bool StatHolder::hasAnyConditions() {
    return conditions.any();
}
