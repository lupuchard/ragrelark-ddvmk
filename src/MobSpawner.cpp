#include "MobSpawner.h"

bool operator<(const RandItemType& left, const RandItemType& right) {
    if (left.getItemBase() < right.getItemBase()) {
        return true;
    } else if (left.getItemBase() > right.getItemBase()) {
        return false;
    } else if (left.getMin() < right.getMin()) {
        return true;
    } else if (left.getMin() > right.getMin()) {
        return false;
    } else if (left.getMax() < right.getMax()) {
        return true;
    }
    return false;
}

MobSpawner::MobSpawner(EnvironmentManager* em) {
    enviroManager = em;
}

MobSpawner::~MobSpawner() {
    for (unsigned int i = 0; i < spawnings.size(); i++) {
        itemSpawnSets[i];
    }
    itemSpawnSets.clear();
    itemSpawnSetNameMap.clear();

    for (unsigned int i = 0; i < spawnings.size(); i++) {
        for (unsigned int j = 0; j < spawnings[i].encounterLevels.size(); j++) {
            delete spawnings[i].encounterLevels[j];
        }
    }
}

//  TPU
// LD9EM
//#K516FV
//SC402AQ
//ZJ837GW
// OIBHN
//  YRX
#define NUM_ALTS 38
//                     0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P   Q   R   S   T   U   V   W   X   Y   Z   #
int altXs[NUM_ALTS] = {0,  0,  1,  0, -1, -1,  1,  1, -1,  0,  2,  0, -2, -1,  1,  2,  2,  1, -1, -2, -2, -2,  2,  2, -2,  0,  3,  0, -3, -1,  1,  3,  3,  1, -1, -3, -3};
int altYs[NUM_ALTS] = {0,  1,  0, -1,  0,  1,  1, -1, -1,  2,  0, -2,  0,  2,  2,  1, -1, -2, -2, -1,  1,  2,  2, -2, -2,  3,  0, -3,  0,  3,  3,  1, -1, -3, -3, -1,  1};

Location* MobSpawner::getNear(Zone* z, int* x, int* y, bool avoidMobs, int baseHeight) {
    for (int i = 1; i < NUM_ALTS; i++) {
        int ax = altXs[i] + *x;
        int ay = altYs[i] + *y;
        if (ax >= 0 && ay >= 0 && ax < z->getWidth() && ay < z->getWidth()) {
            Location* nextLoc = z->getLocationAt(ax, ay);
            if (!(avoidMobs && nextLoc->hasUnit()) && ((baseHeight == -1 && nextLoc->tile == 0) || (baseHeight >= 0 && nextLoc->height != MAX_HEIGHT && fabs(baseHeight - nextLoc->height) <= 2))) {
                *x = ax;
                *y = ay;
                return nextLoc;
            }
        }
    }
    return NULL;
}

int MobSpawner::addItemSpawnSet(string name) {
    itemSpawnSet* s = new itemSpawnSet();
    int num = itemSpawnSets.size();
    itemSpawnSets.push_back(s);
    itemSpawnSetNameMap[name] = s;
    return num;
}

void MobSpawner::addItemToSpawnSet(unsigned short item, unsigned int weight, int itemSpawnSetI) {
    RandItemType si = RandItemType(item, 1, 1, weight);
    itemSpawnSets[itemSpawnSetI]->insert(si);
}
void MobSpawner::addItemToSpawnSet(unsigned short item, unsigned int weight, unsigned char stackMin, unsigned char stackMax, int itemSpawnSetI) {
    RandItemType si = RandItemType(item, stackMin, stackMax, weight);
    itemSpawnSets[itemSpawnSetI]->insert(si);
}

int MobSpawner::addEnvironment(string name) {
    environment e;
    e.name = name;
    e.encounterLevels = vector<encounterLevel*>();
    e.itemSets = vector<set<itemSpawnSet*> >();
    spawnings.push_back(e);
    return spawnings.size() - 1;
}

void MobSpawner::addEncounters(int type, int level, encounterLevel* encounters) {
    environment* e = &spawnings[type];
    int curLevel = e->encounterLevels.size();
    while(curLevel <= level) {
        e->encounterLevels.push_back(NULL);
        curLevel++;
    }
    e->encounterLevels[level] = encounters;
}

void MobSpawner::addItemsToEncounterLevel(int type, int level, string itemSetName) {
    environment* e = &spawnings[type];
    int curLevel = e->itemSets.size();
    while(curLevel <= level) {
        e->itemSets.push_back(set<itemSpawnSet*>());
        curLevel++;
    }
    e->itemSets[level].insert(itemSpawnSetNameMap[itemSetName]);
}

int MobSpawner::hashMob(string tag) {
    int n = 0;
    for (unsigned int i = 0; i < tag.size(); i++) {
        n += tag[i] * (pow(10, i));
    }
    return n % MAX_MOBS;
}

/* Returns the index if unit in storage. */
int MobSpawner::addMob(string name, string tag, StatHolder* u) {
    static bool done = false;
    if (!done) {
        for (int i = 0; i < MAX_MOBS; i++) {
            taggedMobs[i] = mob("x", NULL);
        }
        done = true;
    }
    mob theMob = mob(name, u);
    int num = hashMob(tag);
    if (taggedMobs[num].first == "x") {
        taggedMobs[num] = theMob;
    } else {
        cout << "HASHMAP ERROR: Collision with tag \"" << tag << "\"into \"" << taggedMobs[num].first << "\"'s " << num << "." << endl;
    }
    u->makeHashMaps();
    return 0;
}

pair<string, StatHolder*> MobSpawner::getMob(string tag) {
    mob theMob = taggedMobs[hashMob(tag)];
    if (theMob.first == "x") cout << "HASHMAP ERROR: Mob with tag \"" << tag << "\" does not exist." << endl;
    return theMob;
}

bool MobSpawner::placeMob(Unit* mob, Zone* z, int x, int y, bool allowAlt) {
    Location* loc = z->getLocationAt(x, y);
    if (loc->hasUnit() || loc->height == MAX_HEIGHT) {
        if (allowAlt) {
            int nx = x;
            int ny = y;
            Location* place = getNear(z, &nx, &ny, true, loc->height);
            if (place) {
                place->unit = mob;
                mob->x = nx;
                mob->y = ny;
                //if (anim == ANIM_MOVELOC) {
                    //rMoveLoc(mob, x, y, nx, ny); TODO okay
                //}
            }
        }
    } else {
        loc->unit = mob;
        return true;
    }
    return false;
}

bool MobSpawner::spawnMobSpeTag(int mobTag, Zone* z, int x, int y, bool allowAlt) {
    return spawnMobSpe(taggedMobs[mobTag], z, x, y, allowAlt);
}

bool MobSpawner::spawnMobSpe(mob m, Zone* z, int x, int y, bool allowAlt) {
    Unit* newUnit = new Unit(m.first, m.second);
    newUnit->x = x;
    newUnit->y = y;
    newUnit->theTime = 0; //should be set later
    //areaUnits.insert(pair<Unit*, Zone*>(newUnit, z));
    return placeMob(newUnit, z, x, y, allowAlt);
}

void MobSpawner::createEncounters(Zone* z, int numEnvironments, short* environments, int level, int howMany, vector<pair<int, int> > possibleLocs, vector<pair<Unit*, Zone*> >* unitsAdded) {
    int totalWeight = 0;
    for (int i = 0; i < numEnvironments; i++) {
        encounterLevel* e = spawnings[environments[i]].encounterLevels[level];
        if (e) {
            for (unsigned int i = 0; i < e->size(); i++) {
                totalWeight += (*e)[i].weight;
            }
        }
    }
    for (int j = 0; j < howMany; j++) {
        //TODO groups and equipment
        double num = (double)rand() / RAND_MAX;
        double current = 0;
        for (int i = 0; i < numEnvironments; i++) {
            encounterLevel* e = spawnings[environments[i]].encounterLevels[level];
            if (e) {
                for (unsigned int i = 0; i < e->size(); i++) {
                    current += (double)(*e)[i].weight / totalWeight;
                    if (num < current) {
                        pair<int, int> location = possibleLocs[rand() %  possibleLocs.size()]; //fooey
                        int min = (*e)[i].mobMod.min;
                        int max = (*e)[i].mobMod.max;
                        int num;
                        if (min == max) num = min;
                        else num = rand() % (max - min + 1) + min;

                        int index = (*e)[i].mobMod.equipsInEquipsType;
                        int foo = 0;
                        if (index == -2) foo = (*e)[i].mobMod.mobEquipSet->getRandIndex();

                        for (int k = 0; k < num; k++) {
                            spawnMobSpe((*e)[i].theMob, z, location.first, location.second);
                            Unit* u = z->getLocationAt(location.first, location.second)->unit;
                            unitsAdded->push_back(pair<Unit*, Zone*>(u, z));
                            int index = (*e)[i].mobMod.equipsInEquipsType;
                            if (index != -3) {
                                MobEquips* equipment = new MobEquips;
                                MobEquipSet* equipSet = (*e)[i].mobMod.mobEquipSet;
                                int* equips = new int[2];
                                if (index == -1) {
                                    equipment->len = equipSet->getRandEquips(equips);
                                } else if (index == -2) {
                                    equipment->len = equipSet->getRandEquipsNear(foo, equips);
                                } else {
                                    equipment->len = equipSet->getRandEquipsNear(index, equips); //here
                                }
                                equipment->equips = new Item[equipment->len];
                                for (int i = 0; i < equipment->len; i++) {
                                    equipment->equips[i] = Item(equips[i]);
                                }
                                delete[] equips;
                                u->equipment = equipment;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

void MobSpawner::createItems(Zone* z, int numEnvironments, short* environments, int level, int howMany, vector<pair<int, int> > possibleLocs) {
    itemSpawnSet allItems;
    unsigned int total = 0;
    for (int i = 0; i < numEnvironments; i++) {
        environment* e = &spawnings[environments[i]];
        if ((unsigned int)level < e->itemSets.size()) {
            for (set<itemSpawnSet*>::iterator i = e->itemSets[level].begin(); i != e->itemSets[level].end(); i++) {
                itemSpawnSet* val = *i;
                allItems.insert(val->begin(), val->end());
            }
        }
    }
    for (itemSpawnSet::iterator i = allItems.begin(); i != allItems.end(); i++) {
        total += i->getWeight();
    }
    for (int j = 0; j < howMany; j++) {
        double r = (double)rand() / (double)RAND_MAX;
        int totalSoFar = 0;
        for (itemSpawnSet::iterator i = allItems.begin(); i != allItems.end(); i++) {
            totalSoFar += i->getWeight();
            double p = (double)totalSoFar / total;
            if (r < p) {
                pair<int, int> coords = possibleLocs[rand() %  possibleLocs.size()]; //fooey
                Item item = i->genItem();
                enviroManager->addItemToPlace(coords.first, coords.second, z, item);
                break;
            }
        }
    }
}
