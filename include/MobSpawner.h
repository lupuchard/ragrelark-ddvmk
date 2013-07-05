#ifndef MOBSPAWNER_H
#define MOBSPAWNER_H

#define MAX_MOBS 1000

#include "EnvironmentManager.h"
#include "EnemyEquipStatHolder.h"
#include "MobEquipSet.h"
#include "RandItemType.h"
#include "Blobber.h"
#include "Swarmer.h"

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

typedef std::pair<std::string, StatHolder*> Mob;
typedef struct {
    unsigned char min;
    unsigned char max;
    char equipsInEquipsType; //-1 = ?, -2 = !, -3 = nope
    char dispersion;
    MobEquipSet* mobEquipSet;
} MobMod;
typedef struct {
    Mob mob;
    unsigned int weight;
    MobMod mobMod;
} EncLevelEnc;
typedef std::vector<EncLevelEnc> EncounterLevel;
bool operator<(const RandItemType& left, const RandItemType& right);
typedef std::set<RandItemType> ItemSpawnSet;
typedef struct {
    std::string name;
    std::vector<EncounterLevel*> encounterLevels;
    std::vector<std::set<ItemSpawnSet*> > itemSets;
} Environment;

class MobSpawner {
    public:
        MobSpawner(EnvironmentManager* enviroManager);
        virtual ~MobSpawner();

        Location* getNear(Zone* z, Coord* pos, bool avoidMobs, int baseHeight); //returns a location near those coords, and stores the new coords in the given coords

        int hashMob(std::string tag);
        int addMob(std::string s, std::string tag, StatHolder* u);
        bool placeMob(Unit* unit, Zone* z, Coord pos, bool allowAlt = true);
        Unit* spawnMobSpeTag(int mobI, Zone* z, Coord pos, bool allowAlt = true); //time
        Unit* spawnMobSpe(Mob m, Zone* z, Coord pos, bool allowAlt = true); //time
        std::pair<std::string, StatHolder*> getMob(std::string tag);

        int addEnvironment(std::string name);
        void addEncounters(int type, int level, EncounterLevel* encounters);
        void addItemsToEncounterLevel(int type, int level, std::string itemSetName);

        int addItemSpawnSet(std::string name);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, int itemSpawnSet);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, unsigned char stackMin, unsigned char stackMax, int itemSpawnSet);

        void createEncounters(Zone* z, int numEnvironments, short* environments, int level, int howMany, std::vector<Coord> possibleLocs, std::vector<std::pair<Unit*, Zone*> >* unitsAdded);
        void createItems(Zone* z, int numEnvironments, short* environments, int level, int howMany, std::vector<Coord> possibleLocs);
        void overgrowth(Zone* zone, GenType genType, Coord start, Coord end);
    protected:
    private:
        EnvironmentManager* enviroManager;

        Mob taggedMobs[MAX_MOBS];
        std::vector<Environment> spawnings;

        std::vector<ItemSpawnSet*> itemSpawnSets;
        std::map<std::string, ItemSpawnSet*> itemSpawnSetNameMap;

        Blobber blobber;
};

#endif // MOBSPAWNER_H
