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

#ifndef MOBSPAWNER_H
#define MOBSPAWNER_H

#define MAX_MOBS 1000

#include "EnvironmentManager.h"
#include "EnemyEquipStatHolder.h"
#include "MobEquipSet.h"
#include "RandItemType.h"
#include "Blobber.h"
#include "Swarmer.h"
#include "Texture.h"

typedef std::pair<String, StatHolder*> Mob;
typedef struct {
    unsigned char min;
    unsigned char max;
    char equipsInEquipsType; //-1 = random, -2 = weighted, -3 = nope
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
    String name;
    std::vector<EncounterLevel*> encounterLevels;
    std::vector<std::set<ItemSpawnSet*> > itemSets;
} Environment;

class MobSpawner {
    public:
        MobSpawner(EnvironmentManager* enviroManager);
        virtual ~MobSpawner();

        Location* getNear(Zone* z, Coord* pos, bool avoidMobs, int baseHeight); //returns a location near those coords, and stores the new coords in the given coords

        void parseMob(YAML::Node fileNode);
        void parseDefaultStats(YAML::Node fileNode);

        void addMob(String s, StatHolder* u);
        bool placeMob(Unit* unit, Zone* z, Coord pos, bool allowAlt = true);
        Unit* spawnMob(Mob m, Zone* z, Coord pos, int time = 0, bool allowAlt = true);
        Mob getMob(short index);
        Mob getMob(String name);
        bool mobExists(String name);

        int addEnvironment(String name);
        void addEncounters(int type, int level, EncounterLevel* encounters);
        void addItemsToEncounterLevel(int type, int level, String itemSetName);
        int getEnvironment(String name);

        void parseRarities(YAML::Node fileNode);
        void parseItems(YAML::Node fileNode);
        int addItemSpawnSet(String name);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, int itemSpawnSet);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, unsigned char stackMin, unsigned char stackMax, int itemSpawnSet);

        void createEncounters(Zone* z, int numEnvironments, short* environments, int level, int howMany, std::vector<Coord> possibleLocs, std::vector<std::pair<Unit*, Zone*> >* unitsAdded);
        void createItems(Zone* z, int numEnvironments, short* environments, int level, int howMany, std::vector<Coord> possibleLocs);
        void overgrowth(Zone* zone, GenType genType, Coord start, Coord end);

        void parseSpawn(YAML::Node fileNode);
    private:
        EncLevelEnc parseSpawnMob(YAML::Node fileNode, Mob mob);

        EnvironmentManager* enviroManager;

        std::vector<Mob> mobs;
        std::map<String, short> mobNameMap;
        std::vector<Stat*> defaultStats;

        std::vector<Environment> spawnings;
        std::map<String, int> enviroNameMap;

        std::map<String, int> rarityMap;
        std::vector<ItemSpawnSet*> itemSpawnSets;
        std::map<String, ItemSpawnSet*> itemSpawnSetNameMap;

        Blobber blobber;
};

#endif // MOBSPAWNER_H
