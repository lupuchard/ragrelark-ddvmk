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
    for (int i = 0; i < MAX_MOBS; i++) {
        taggedMobs[i] = mob("x", NULL);
    }
}

MobSpawner::~MobSpawner() {
    for (unsigned int i = 0; i < itemSpawnSets.size(); i++) {
        delete itemSpawnSets[i];
    }
    itemSpawnSets.clear();
    itemSpawnSetNameMap.clear();

    for (unsigned int i = 0; i < spawnings.size(); i++) {
        for (unsigned int j = 0; j < spawnings[i].encounterLevels.size(); j++) {
            delete spawnings[i].encounterLevels[j];
        }
    }

    for (int i = 0; i < MAX_MOBS; i++) {
        if (taggedMobs[i].first != "x") delete taggedMobs[i].second;
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
            if (!(avoidMobs && nextLoc->hasUnit()) && !getTile(nextLoc->tile)->blocksMove() &&
                    (baseHeight >= 0 && nextLoc->height != MAX_HEIGHT && fabs(baseHeight - nextLoc->height) <= 2)) {
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
                return true;
            }
        }
    } else {
        loc->unit = mob;
        return true;
    }
    return false;
}

Unit* MobSpawner::spawnMobSpeTag(int mobTag, Zone* z, int x, int y, bool allowAlt) {
    return spawnMobSpe(taggedMobs[mobTag], z, x, y, allowAlt);
}

Unit* MobSpawner::spawnMobSpe(mob m, Zone* z, int x, int y, bool allowAlt) {
    Unit* newUnit;
    if (m.second->getStatValue(S_SWARM)) {
        newUnit = new Swarmer(m.first, m.second);
    } else {
        newUnit = new Unit(m.first, m.second);
    }
    newUnit->x = x;
    newUnit->y = y;
    newUnit->theTime = 0; //should be set later
    if (placeMob(newUnit, z, x, y, allowAlt)) {
        return newUnit;
    }
    cout << "HAPPENS?" << endl;
    delete newUnit;
    return NULL;
}

void MobSpawner::createEncounters(Zone* z, int numEnvironments, short* environments, int level, int howMany, vector<pair<int, int> > possibleLocs, vector<pair<Unit*, Zone*> >* unitsAdded) {
    //first it calculates the total weight by adding up the weights of all spawnable mobs
    int totalWeight = 0;
    for (int i = 0; i < numEnvironments; i++) {
        encounterLevel* e = spawnings[environments[i]].encounterLevels[level];
        if (e) {
            for (unsigned int i = 0; i < e->size(); i++) {
                totalWeight += (*e)[i].weight;
            }
        }
    }

    //it spawns as many mobs as it needs to
    for (int j = 0; j < howMany; j++) {
        double sel = (double)rand() / RAND_MAX;
        double current = 0;
        for (int i = 0; i < numEnvironments; i++) {
            encounterLevel* e = spawnings[environments[i]].encounterLevels[level];
            if (e) {
                for (unsigned int i = 0; i < e->size(); i++) {
                    current += (double)(*e)[i].weight / totalWeight;
                    if (sel < current) {
                        //this mob has been chosen to spawn

                        pair<int, int> location = possibleLocs[rand() %  possibleLocs.size()]; //first it selects a random position
                        int min = (*e)[i].mobMod.min;
                        int max = (*e)[i].mobMod.max;
                        int num;
                        if (min == max) num = min;
                        else num = rand() % (max - min + 1) + min; //then it selects how many to spawn if they are spawning in a group

                        int index = (*e)[i].mobMod.equipsInEquipsType;
                        int foo = 0;
                        if (index == -2) foo = (*e)[i].mobMod.mobEquipSet->getRandIndex();

                        for (int k = 0; k < num; k++) {
                            Unit* u = spawnMobSpe((*e)[i].theMob, z, location.first, location.second);
                            if (u) {
                                unitsAdded->push_back(pair<Unit*, Zone*>(u, z));
                                if (index != -3) {
                                    MobEquips* equipment = new MobEquips;
                                    MobEquipSet* equipSet = (*e)[i].mobMod.mobEquipSet;
                                    int* equips = new int[2];
                                    if (index == -1) {
                                        equipment->len = equipSet->getRandEquips(equips);
                                    } else if (index == -2) {
                                        equipment->len = equipSet->getRandEquipsNear(foo, equips);
                                    } else {
                                        equipment->len = equipSet->getRandEquipsNear(index, equips);
                                    }
                                    equipment->equips = new Item[equipment->len];
                                    for (int i = 0; i < equipment->len; i++) {
                                        equipment->equips[i] = Item(equips[i]);
                                    }
                                    delete[] equips;
                                    if (equipment->len) {
                                        u->equipment = equipment;
                                    }
                                }
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

void MobSpawner::overgrowth(Zone* zone, GenType genType, int sx, int sy, int ex, int ey) {
    static const int plantTag = hashMob("2growt");
    if (genType == GEN_DUNGEON) {
        int num = (ex - sx) * (ey - sy) / 1000;
        for (int i = 0; i < num; i++) {
            int x = rand() % zone->getWidth();
            int y = rand() % zone->getHeight(); //random location
            int rad = rand() % 4 + 2; //random size
            blobber.makeCircle(rad);
            const bool** circle = blobber.getBlob();
            for (int i = 0; i < blobber.getWidth(); i++) {
                for (int j = 0; j < blobber.getHeight(); j++) {
                    if (circle[i][j]) {
                        int xi = x + i - rad;
                        int yj = y + j - rad;
                        if (xi >= sx && xi < ex && yj >= sy && yj < ey) {
                            Location* locAt = zone->getLocationAt(xi, yj);
                            if (locAt->height != MAX_HEIGHT && locAt->height > MAX_HEIGHT / 4 && !locAt->hasUnit() && locAt->structure == S_NONE) {
                                spawnMobSpeTag(plantTag, zone, xi, yj, true);
                            }
                        }
                    }
                }
            }
        }
    }
}
