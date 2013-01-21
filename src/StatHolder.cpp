#include "StatHolder.h"

#define NUM_STAT_HOLDERS_SOFAR 3

#include <stdexcept>
#include <exception>

FormulaUser* formUser;
void setFormUser(FormulaUser* fUser) {
    formUser = fUser;
}

map<completeStat, vector<completeStat>*> afflictions;
map<int, vector<completeStat>*> conAfflictions;
set<Stat*> itemAfflictions;
set<Stat*> enemyAfflictions;
map<SkillType, set<Stat*> > skillAfflictions;

set<Stat*> getItemAfflictions() {
    return itemAfflictions;
}
set<Stat*> getEnemyAfflictions() {
    return enemyAfflictions;
}
set<Stat*> getSkillAfflictions(SkillType skill) {
    return skillAfflictions[skill];
}
vector<completeStat>* getAfflictions(Stat* s, VOwner ownertype) {
    completeStat cStat = pair<VOwner, Stat*>(ownertype, s);
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


void addAffliction(completeStat afflictingStat, Stat* afflictedStat, VOwner edOwner) {
    if (edOwner == V_UNIT) {
        switch(afflictingStat.first) {
            case V_ITEM: itemAfflictions.insert(afflictedStat); break;
            case V_ENEMY: enemyAfflictions.insert(afflictedStat); break;
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

void addConAffliction(int afflictingCondition, VOwner afflictingConOwner, Stat* afflictedStat, VOwner edOwner) {
    if (edOwner == V_UNIT) {
        switch(afflictingConOwner) {
            case V_ENEMY: enemyAfflictions.insert(afflictedStat); break;
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

void addSkillAffliction(SkillType afflictingSkill, Stat* afflictedStat) {
    map<SkillType, set<Stat*> >::iterator i = skillAfflictions.find(afflictingSkill);
    if (i == skillAfflictions.end()) {
        skillAfflictions[afflictingSkill] = set<Stat*>();
        skillAfflictions[afflictingSkill].insert(afflictedStat);
    } else {
        i->second.insert(afflictedStat);
    }
}

StatHolder::StatHolder(VOwner o) {
    owner = o;
    numIntStats = 0;
    numFloatStats = 0;
    intStats = new unsigned char[0];
    intValues = new short[0];
    floatStats = new unsigned char[0];
    floatValues = new float[0];
    aThis = this;
    hashMapped = false;
    tempHashMap = NULL;
    tempFloatHashMap = NULL;
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
    if (hashMapped) {
        (*tempHashMap)[(unsigned char)stat] = pair<short, bool>((short)value, true);
    }
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
    if (hashMapped) {
        (*tempFloatHashMap)[stat] = pair<float, bool>(value, true);
    }
}

void StatHolder::needToUpdate(int stat, bool isFloat) {
    if (hashMapped) {
        if (isFloat) {
            (*tempFloatHashMap)[stat].second = true;
        } else {
            (*tempHashMap)[stat].second = true;
        }
    } else {
        if (isFloat) {
            int i = binarySearchFloat(0, numFloatStats - 1, stat);
            toBeUpdatedFloat[i] = true;
        } else {
            int i = binarySearchInt(0, numIntStats - 1, stat);
            toBeUpdatedInt[i] = true;
        }
    }
    statChanged(stat);
}

short StatHolder::getStatValue(int stat) {
    if (hashMapped) {
        unordered_map<unsigned char, pair<short, bool> >::iterator i = tempHashMap->find(stat);
        if (i == tempHashMap->end()) return 0;
        pair<short, bool> val = i->second;
        if (val.second) {
            Stat* s = getStat((VOwner)owner, stat);
            int temp = val.first;
            val.first = s->getFormula()->run(formUser, aThis, val.first);
            if (temp != val.first) {
                formUser->statChanged(stat, aThis);
            }
            val.second = false;
            (*tempHashMap)[stat] = val;
        }
        return val.first;
    }
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
    if (hashMapped) {
        unordered_map<unsigned char, pair<float, bool> >::iterator i = tempFloatHashMap->find(stat);
        if (i == tempFloatHashMap->end()) return 0;
        pair<float, bool> val = i->second;
        if (val.second) {
            Stat* s = getStat((VOwner)owner, stat);
            float temp = val.first;
            val.first = s->getFormula()->runFloat(formUser, aThis, val.first);
            if (temp != val.first) {
                formUser->statChanged(stat, aThis);
            }
            val.second = false;
            (*tempFloatHashMap)[stat] = val;
        }
        return val.first;
    }
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
    if (hashMapped) {
        (*tempHashMap)[stat] = pair<short, bool>(value, true);
        formUser->statChanged(stat, aThis);
    } else {
        int i = binarySearchInt(0, numIntStats - 1, stat);
        if (i != -1) {
            intValues[i] = value;
            formUser->statChanged(stat, aThis);
            toBeUpdatedInt[i] = true;
        }
    }
}

void StatHolder::setStatF(int stat, float value) {
    if (hashMapped) {
        (*tempFloatHashMap)[stat] = pair<float, bool>(value, true);
        formUser->statChanged(stat, aThis);
    } else {
        int i = binarySearchFloat(0, numFloatStats - 1, stat);
        if (i != -1) {
            floatValues[i] = value;
            formUser->statChanged(stat, aThis);
            toBeUpdatedFloat[i] = true;
        }
    }
}

void StatHolder::statChanged(int stat) {
    formUser->statChanged(stat, aThis);
}

short StatHolder::modifyStat(int stat, int amount) {
    if (hashMapped) {
        pair<short, bool> v = (*tempHashMap)[stat];
        v.first += amount;
        v.second = true;
        (*tempHashMap)[stat] = v;
        return v.first;
    } else {
        int i = binarySearchInt(0, numIntStats - 1, stat);
        if (i != -1) {
            intValues[i] = intValues[i] + amount;
            formUser->statChanged(stat, aThis);
            toBeUpdatedInt[i] = true;
            return intValues[i];
        }
    }
    return 0;
}

float StatHolder::modifyStatF(int stat, float amount) {
    if (hashMapped) {
        pair<float, bool> v = (*tempFloatHashMap)[stat];
        v.first += amount;
        v.second = true;
        (*tempFloatHashMap)[stat] = v;
        return v.first;
    } else {
        int i = binarySearchFloat(0, numFloatStats - 1, stat);
        if (i != -1) {
            floatValues[i] = floatValues[i] + amount;
            formUser->statChanged(stat, aThis);
            toBeUpdatedFloat[i] = true;
            return floatValues[i];
        }
    }
    return 0;
}

bool StatHolder::hasStat(int stat, bool isFloat) {
    if (hashMapped) {
        if (isFloat) {
            return tempFloatHashMap->count(stat);
        }
        return tempHashMap->count(stat);
    } else {
        if (isFloat) {
            return (binarySearchFloat(0, numFloatStats - 1, stat) != -1);
        }
        return (binarySearchInt(0, numIntStats - 1, stat) != -1);
    }
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
    formUser->conditionChanged(conditionI, aThis);
}

bool StatHolder::hasAnyConditions() {
    return conditions.any();
}

void StatHolder::makeHashMaps() {
    if (!hashMapped) {
        tempHashMap = new unordered_map<unsigned char, pair<short, bool> >();
        tempFloatHashMap = new unordered_map<unsigned char, pair<float, bool> >();
        for (int i = 0; i < numIntStats; i++) {
            (*tempHashMap)[intStats[i]] = pair<short, bool>(intValues[i], toBeUpdatedInt[i]);
        }
        for (int i = 0; i < numFloatStats; i++) {
            (*tempFloatHashMap)[floatStats[i]] = pair<float, bool>(floatValues[i], toBeUpdatedFloat[i]);
        }
        hashMapped = true;
    }
}

void StatHolder::removeHashMaps() {
    if (hashMapped) {
        hashMapped = false;
        for (unordered_map<unsigned char, pair<short, bool> >::iterator i = tempHashMap->begin(); i != tempHashMap->end(); ++i) {
            setStat(i->first, i->second.first);
            toBeUpdatedInt[i->first] = i->second.second;
        }
        for (unordered_map<unsigned char, pair<float, bool> >::iterator i = tempFloatHashMap->begin(); i != tempFloatHashMap->end(); ++i) {
            setStatF(i->first, i->second.first);
            toBeUpdatedFloat[i->first] = i->second.second;
        }
        delete tempHashMap;
        delete tempFloatHashMap;
    }
}
