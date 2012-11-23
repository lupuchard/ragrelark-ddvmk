#include "Start.h"



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

typedef set<pair<unsigned short, unsigned short> > itemSpawnSet; //item type, weight

typedef vector<pair<mob, unsigned int> > encounterLevel; //the unsigned char is the weight

typedef struct {
    string name;
    vector<encounterLevel*> encounterLevels;
    vector<set<itemSpawnSet*> > itemSets;
} environment;
#define MAX_MOBS 1000
mob taggedMobs[MAX_MOBS];
vector<environment> spawnings;

vector<itemSpawnSet*> itemSpawnSets;
map<string, itemSpawnSet*> itemSpawnSetNameMap;

void Start::cleanSpawnData() {
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

int Start::addItemSpawnSet(string name) {
    itemSpawnSet* s = new itemSpawnSet();
    int num = itemSpawnSets.size();
    itemSpawnSets.push_back(s);
    itemSpawnSetNameMap[name] = s;
    return num;
}

void Start::addItemToSpawnSet(unsigned short item, unsigned short weight, int itemSpawnSet) {
    itemSpawnSets[itemSpawnSet]->insert(pair<unsigned short, unsigned short>(item, weight));
}

int Start::addEnvironment(string name) {
    environment e;
    e.name = name;
    e.encounterLevels = vector<encounterLevel*>();
    e.itemSets = vector<set<itemSpawnSet*> >();
    spawnings.push_back(e);
    return spawnings.size() - 1;
}

void Start::addEncounters(int type, int level, encounterLevel* encounters) {
    environment* e = &spawnings[type];
    int curLevel = e->encounterLevels.size();
    while(curLevel <= level) {
        e->encounterLevels.push_back(NULL);
        curLevel++;
    }
    e->encounterLevels[level] = encounters;
}

void Start::addItemsToEncounterLevel(int type, int level, string itemSetName) {
    environment* e = &spawnings[type];
    int curLevel = e->itemSets.size();
    while(curLevel <= level) {
        e->itemSets.push_back(set<itemSpawnSet*>());
        curLevel++;
    }
    e->itemSets[level].insert(itemSpawnSetNameMap[itemSetName]);
}

int Start::hashMob(string tag) {
    int n = 0;
    for (unsigned int i = 0; i < tag.size(); i++) {
        n += tag[i] * (pow(10, i));
    }
    return n % MAX_MOBS;
}

/* Returns the index if unit in storage. */
int Start::addMob(string name, string tag, StatHolder* u) {
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

pair<string, StatHolder*> Start::getMob(string tag) {
    mob theMob = taggedMobs[hashMob(tag)];
    if (theMob.first == "x") cout << "HASHMAP ERROR: Mob with tag \"" << tag << "\" does not exist." << endl;
    return theMob;
}

bool Start::placeMob(Unit* mob, Zone* z, int x, int y, bool allowAlt, int anim) {
    Location* loc = z->getLocationAt(x, y);
    if (loc->hasUnit() || loc->height == MAX_HEIGHT) {
        if (allowAlt) {
            for (int i = 1; i < NUM_ALTS; i++) {
                int ax = altXs[i] + x;
                int ay = altYs[i] + y;
                if (ax >= 0 && ay >= 0 && ax < z->getWidth() && ay < z->getWidth()) {
                    Location* nextLoc = z->getLocationAt(ax, ay);
                    if (!nextLoc->hasUnit() && nextLoc->height != MAX_HEIGHT && fabs(loc->height - nextLoc->height) <= 2) {
                        nextLoc->unit = mob;
                        mob->x = ax;
                        mob->y = ay;
                        if (anim == A_MOVELOC) {
                            rMoveLoc(mob, x, y, ax, ay);
                        }
                        return true;
                    }
                }
            }
        }
    } else {
        loc->unit = mob;
        return true;
    }
    return false;
}

bool Start::spawnMobSpeTag(int mobTag, Zone* z, int x, int y, bool allowAlt, int anim) {
    return spawnMobSpe(taggedMobs[mobTag], z, x, y, allowAlt, anim);
}

bool Start::spawnMobSpe(mob m, Zone* z, int x, int y, bool allowAlt, int anim) {
    Unit* newUnit = new Unit(m.first, m.second);
    newUnit->x = x;
    newUnit->y = y;
    newUnit->theTime = player->getUnit()->theTime;
    areaUnits.insert(pair<Unit*, Zone*>(newUnit, z));
    return placeMob(newUnit, z, x, y, allowAlt, anim);
}

void Start::createEncounters(Zone* z, int howMany, vector<pair<int, int> > possibleLocs) {
    encounterLevel* e = spawnings[z->getMobType()].encounterLevels[z->getMobSpawnLevel()];
    int totalWeight = 0;
    for (unsigned int i = 0; i < e->size(); i++) {
        totalWeight += (*e)[i].second;
    }
    for (int j = 0; j < howMany; j++) {
        double num = (double)rand() / RAND_MAX;
        double current = 0;
        for (unsigned int i = 0; i < e->size(); i++) {
            current += (double)(*e)[i].second / totalWeight;
            if (num < current) {
                pair<int, int> location = possibleLocs[rand() %  possibleLocs.size()]; //fooey
                spawnMobSpe((*e)[i].first, z, location.first, location.second, true, A_NONE);
                break;
            }
        }
    }
}

void Start::createItems(Zone* z, int howMany, vector<pair<int, int> > possibleLocs) {
    environment* e = &spawnings[z->getMobType()];
    int level = z->getMobSpawnLevel();
    itemSpawnSet allItems;
    int total = 0;
    for (set<itemSpawnSet*>::iterator i = e->itemSets[level].begin(); i != e->itemSets[level].end(); i++) {
        itemSpawnSet* val = *i;
        allItems.insert(val->begin(), val->end());
    }
    for (itemSpawnSet::iterator i = allItems.begin(); i != allItems.end(); i++) {
        total += i->second;
    }
    for (int j = 0; j < howMany; j++) {
        double r = (double)rand() / (double)RAND_MAX;
        int totalSoFar = 0;
        for (itemSpawnSet::iterator i = allItems.begin(); i != allItems.end(); i++) {
            totalSoFar += i->second;
            double p = (double)totalSoFar / total;
            if (r < p) {
                pair<int, int> coords = possibleLocs[rand() %  possibleLocs.size()]; //fooey
                //Location* location = z->getLocationAt(coords.first, coords.second);
                Item item = Item(i->first);
                addItemToPlace(coords.first, coords.second, z, item);
                //location->addItem(item);
                break;
            }
        }
    }
}
