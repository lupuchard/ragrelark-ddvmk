#ifndef MOBSPAWNER_H
#define MOBSPAWNER_H

#define MAX_MOBS 1000

#include "EnvironmentManager.h"
#include "EnemyEquipStatHolder.h"
#include "MobEquipSet.h"
#include "RandItemType.h"
#include "Blobber.h"

typedef pair<string, StatHolder*> mob;
typedef struct {
    unsigned char min;
    unsigned char max;
    char equipsInEquipsType; //-1 = ?, -2 = !, -3 = nope
    char dispersion;
    MobEquipSet* mobEquipSet;
} MobMod;
typedef struct {
    mob theMob;
    unsigned int weight;
    MobMod mobMod;
} EncLevelEnc;
typedef vector<EncLevelEnc> encounterLevel;
bool operator<(const RandItemType& left, const RandItemType& right);
typedef set<RandItemType> itemSpawnSet;
typedef struct {
    string name;
    vector<encounterLevel*> encounterLevels;
    vector<set<itemSpawnSet*> > itemSets;
} environment;

class MobSpawner {
    public:
        MobSpawner(EnvironmentManager* enviroManager);
        virtual ~MobSpawner();

        Location* getNear(Zone* z, int* x, int* y, bool avoidMobs, int baseHeight); //returns a location near those coords, and stores the new coords in the given coords

        int hashMob(string tag);
        int addMob(string s, string tag, StatHolder* u);
        bool placeMob(Unit* unit, Zone* z, int x, int y, bool allowAlt = true);
        Unit* spawnMobSpeTag(int mobI, Zone* z, int x, int y, bool allowAlt = true); //time
        Unit* spawnMobSpe(mob m, Zone* z, int x, int y, bool allowAlt = true); //time
        pair<string, StatHolder*> getMob(string tag);

        int addEnvironment(string name);
        void addEncounters(int type, int level, encounterLevel* encounters);
        void addItemsToEncounterLevel(int type, int level, string itemSetName);

        int addItemSpawnSet(string name);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, int itemSpawnSet);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, unsigned char stackMin, unsigned char stackMax, int itemSpawnSet);

        void createEncounters(Zone* z, int numEnvironments, short* environments, int level, int howMany, vector<pair<int, int> > possibleLocs, vector<pair<Unit*, Zone*> >* unitsAdded);
        void createItems(Zone* z, int numEnvironments, short* environments, int level, int howMany, vector<pair<int, int> > possibleLocs);
        void overgrowth(Zone* zone, GenType genType, int sx, int sy, int ex, int ey);
    protected:
    private:
        EnvironmentManager* enviroManager;

        mob taggedMobs[MAX_MOBS];
        vector<environment> spawnings;

        vector<itemSpawnSet*> itemSpawnSets;
        map<string, itemSpawnSet*> itemSpawnSetNameMap;

        Blobber blobber;
};

#endif // MOBSPAWNER_H
