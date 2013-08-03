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
    //TODO not done
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

Location* MobSpawner::getNear(Zone* z, Coord* pos, bool avoidMobs, int baseHeight) {
    for (int i = 1; i < NUM_ALTS; i++) {
        Coord a = *pos + Coord(altXs[i], altYs[i]);
        if (a.inBounds(z->getWidth(), z->getHeight())) {
            Location* nextLoc = z->getLocationAt(a);
            if (!(avoidMobs && nextLoc->hasUnit()) && !Tile::get(nextLoc->tile)->blocksMove() &&
                    (baseHeight >= 0 && nextLoc->height != MAX_HEIGHT && fabs(baseHeight - nextLoc->height) <= 2)) {
                *pos = a;
                return nextLoc;
            }
        }
    }
    return NULL;
}

void MobSpawner::parseRarities(YAML::Node fileNode, std::ostream& lerr) {
    for (YAML::const_iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter) {
        rarityMap[iter->first.as<String>()] = iter->second.as<int>();
    }
}

void MobSpawner::parseItems(YAML::Node fileNode, std::ostream& lerr) {
    for (YAML::const_iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter) {
        String setName = iter->first.as<String>();
        int itemSpawnSetI = addItemSpawnSet(setName);
        YAML::Node issNode = iter->second;
        for (YAML::const_iterator jter = issNode.begin(); jter != issNode.end(); ++jter) {
            YAML::Node itNode = *jter;
            String itemName = readYAMLStr(itNode, "Item", "", "Item expected (" + setName + ").", lerr);
            if (ItemType::has(itemName)) {
                unsigned short item = ItemType::get(itemName)->getIndex();
                String rarityName = readYAMLStr(itNode, "Rarity", "common");
                int weight;
                if (isNum(rarityName)) {
                    weight = sti(rarityName);
                } else {
                    weight = rarityMap[rarityName];
                }
                if (itNode["Quantity"]) {
                    YAML::Node n = itNode["Quantity"];
                    int min = n[0].as<int>();
                    int max;
                    if (n[1]) max = n[1].as<int>();
                    else max = min;
                    addItemToSpawnSet(item, weight, max, min, itemSpawnSetI);
                } else {
                    addItemToSpawnSet(item, weight, itemSpawnSetI);
                }
            } else {
                lerr << "'" << itemName << "' is not an existing item.\n";
            }
        }
    }
}

int MobSpawner::addItemSpawnSet(String name) {
    ItemSpawnSet* s = new ItemSpawnSet();
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

int MobSpawner::addEnvironment(String name) {
    enviroNameMap[name] = spawnings.size();
    Environment e;
    e.name = name;
    e.encounterLevels = std::vector<EncounterLevel*>();
    e.itemSets = std::vector<std::set<ItemSpawnSet*> >();
    spawnings.push_back(e);
    return spawnings.size() - 1;
}
int MobSpawner::getEnvironment(String name) {
    return enviroNameMap[name];
}

void MobSpawner::addEncounters(int type, int level, EncounterLevel* encounters) {
    Environment* e = &spawnings[type];
    int curLevel = e->encounterLevels.size();
    while(curLevel <= level) {
        e->encounterLevels.push_back(NULL);
        curLevel++;
    }
    e->encounterLevels[level] = encounters;
}

void MobSpawner::addItemsToEncounterLevel(int type, int level, String itemSetName) {
    Environment* e = &spawnings[type];
    int curLevel = e->itemSets.size();
    while(curLevel <= level) {
        e->itemSets.push_back(std::set<ItemSpawnSet*>());
        curLevel++;
    }
    e->itemSets[level].insert(itemSpawnSetNameMap[itemSetName]);
}

bool MobSpawner::placeMob(Unit* mob, Zone* z, Coord pos, bool allowAlt) {
    Location* loc = z->getLocationAt(pos);
    if (loc->hasUnit() || loc->height == MAX_HEIGHT) {
        if (allowAlt) {
            Coord n = pos;
            Location* place = getNear(z, &n, true, loc->height);
            if (place) {
                place->unit = mob;
                mob->pos = n;
                return true;
            }
        }
    } else {
        loc->unit = mob;
        return true;
    }
    return false;
}

Unit* MobSpawner::spawnMob(Mob* m, Zone* z, Coord pos, int time, bool allowAlt) {
    Unit* newUnit;
    // create the unit, depending on whether its a swarmer or not
    if (m->proto->getStatValue(Stat::SWARM)) {
        newUnit = new Swarmer(m->name, m->proto);
    } else {
        newUnit = new Unit(m->name, m->proto);
    }
    //place the unit
    newUnit->pos = pos;
    newUnit->theTime = time;
    if (placeMob(newUnit, z, pos, allowAlt)) {
        //if the unit has a pet, spawn the pet too
        int pet = m->proto->getStatValue(Stat::PET);
        if (allowAlt && pet && !(rand() % 30)) spawnMob(Unit::getMob(pet), z, pos, time, true);
        return newUnit;
    }
    // if placing failed, delete mob
    delete newUnit;
    return NULL;
}

void MobSpawner::createEncounters(Zone* z, int numEnvironments, short* environments, int level, int howMany, std::vector<Coord> possibleLocs, std::vector<std::pair<Unit*, Zone*> >* unitsAdded) {
    std::set<BlobGen*> blobs;

    //first it calculates the total weight by adding up the weights of all spawnable mobs
    int totalWeight = 0;
    for (int i = 0; i < numEnvironments; i++) {
        EncounterLevel* e = spawnings[environments[i]].encounterLevels[level];
        if (e) {
            for (unsigned int i = 0; i < e->size(); i++) {
                totalWeight += (*e)[i].weight;
            }
        }
        blobs.insert(spawnings[environments[i]].blobgens.begin(), spawnings[environments[i]].blobgens.end());
    }

    //it spawns as many mobs as it needs to
    for (int j = 0; j < howMany; j++) {
        double sel = (double)rand() / RAND_MAX;
        double current = 0;
        for (int i = 0; i < numEnvironments; i++) {
            EncounterLevel* e = spawnings[environments[i]].encounterLevels[level];
            if (e) {
                for (unsigned int i = 0; i < e->size(); i++) {
                    current += (double)(*e)[i].weight / totalWeight;
                    if (sel < current) {
                        //this mob has been chosen to spawn

                        Coord location = possibleLocs[rand() %  possibleLocs.size()]; //first it selects a random position
                        int min = (*e)[i].mobMod.min;
                        int max = (*e)[i].mobMod.max;
                        int num;
                        if (min == max) num = min;
                        else num = rand() % (max - min + 1) + min; //then it selects how many to spawn if they are spawning in a group

                        int index = (*e)[i].mobMod.equipsInEquipsType;
                        int foo = 0;
                        if (index == -2) foo = (*e)[i].mobMod.mobEquipSet->getRandIndex();

                        for (int k = 0; k < num; k++) {
                            Unit* u = spawnMob((*e)[i].mob, z, location);
                            if (u) {
                                unitsAdded->push_back(std::pair<Unit*, Zone*>(u, z));
                                if (index != -3) {
                                    MobEquips* equipment = new MobEquips;
                                    MobEquipSet* equipSet = (*e)[i].mobMod.mobEquipSet;
                                    std::vector<ItemType*> equips;
                                    if (index == -1) {
                                        equipSet->getRandEquips(equips);
                                    } else if (index == -2) {
                                        equipSet->getRandEquipsNear(foo, equips);
                                    } else {
                                        equipSet->getRandEquipsNear(index, equips);
                                    }
                                    equipment->len = equips.size();
                                    equipment->equips = new Item[equipment->len];
                                    for (int i = 0; i < equipment->len; i++) {
                                        equipment->equips[i] = Item(equips[i]);
                                    }
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

    for (std::set<BlobGen*>::iterator iter = blobs.begin(); iter != blobs.end(); ++iter) {
        BlobGen* b = *iter;
        unsigned int amount;
        if (b->minAmount == b->maxAmount) amount = b->minAmount;
        else amount = rand() % (b->maxAmount - b->minAmount + 1) + b->minAmount;
        for (unsigned int i = 0; i < amount; i++) {
            Coord loc = Coord(rand() % z->getWidth(), rand() % z->getHeight());
            unsigned int size;
            if (b->minSize == b->maxSize) size = b->minSize;
            else size = rand() % (b->maxSize - b->minSize + 1) + b->minSize;
            switch(b->shape) {
                case BLOB_CIRCLE: blobber.makeCircle(size); break;
                default: break;
            }
            const bool** blob = blobber.getBlob();
            for (int i = 0; i < blobber.getWidth(); i++) {
                for (int j = 0; j < blobber.getHeight(); j++) {
                    if (blob[i][j]) {
                        Coord c = Coord(loc.x + i - size, loc.y + j - size);
                        if (c.inBounds(z->getWidth(), z->getHeight())) {
                            Location* locAt = z->getLocationAt(c);
                            if (locAt->height != MAX_HEIGHT && locAt->height > MAX_HEIGHT / 4 && !locAt->hasUnit() && locAt->structure == S_NONE) {
                                spawnMob(b->mob, z, c, 0, false);
                            }
                        }
                    }
                }
            }
        }
    }
}

void MobSpawner::createItems(Zone* z, int numEnvironments, short* environments, int level, int howMany, std::vector<Coord> possibleLocs) {
    ItemSpawnSet allItems;
    unsigned int total = 0;
    for (int i = 0; i < numEnvironments; i++) {
        Environment* e = &spawnings[environments[i]];
        if ((unsigned int)level < e->itemSets.size()) {
            for (std::set<ItemSpawnSet*>::iterator i = e->itemSets[level].begin(); i != e->itemSets[level].end(); ++i) {
                ItemSpawnSet* val = *i;
                allItems.insert(val->begin(), val->end());
            }
        }
    }
    for (ItemSpawnSet::iterator i = allItems.begin(); i != allItems.end(); ++i) {
        total += i->getWeight();
    }
    for (int j = 0; j < howMany; j++) {
        double r = (double)rand() / (double)RAND_MAX;
        int totalSoFar = 0;
        for (ItemSpawnSet::iterator i = allItems.begin(); i != allItems.end(); ++i) {
            totalSoFar += i->getWeight();
            double p = (double)totalSoFar / total;
            if (r < p) {
                Coord coord = possibleLocs[rand() %  possibleLocs.size()]; //fooey
                Item item = i->genItem();
                enviroManager->addItemToPlace(coord, z, item);
                break;
            }
        }
    }
}

void MobSpawner::overgrowth(Zone* zone, GenType genType, Coord start, Coord end) {
    /*static const int plantTag = hashMob("2growt");
    if (genType == GEN_DUNGEON) {
        int num = (end.x - start.x) * (end.y - start.y) / 1000;
        for (int i = 0; i < num; i++) {
            Coord loc = Coord(rand() % zone->getWidth(), rand() % zone->getHeight()); //random location
            int rad = rand() % 4 + 2; //random size
            blobber.makeCircle(rad);
            const bool** circle = blobber.getBlob();
            for (int i = 0; i < blobber.getWidth(); i++) {
                for (int j = 0; j < blobber.getHeight(); j++) {
                    if (circle[i][j]) {
                        int xi = loc.x + i - rad;
                        int yj = loc.y + j - rad;
                        if (xi >= start.x && xi < end.x && yj >= start.y && yj < end.y) {
                            Location* locAt = zone->getLocationAt(Coord(xi, yj));
                            if (locAt->height != MAX_HEIGHT && locAt->height > MAX_HEIGHT / 4 && !locAt->hasUnit() && locAt->structure == S_NONE) {
                                spawnMobSpeTag(plantTag, zone, Coord(xi, yj), true);
                            }
                        }
                    }
                }
            }
        }
    }*/
}

EncLevelEnc MobSpawner::parseSpawnMob(YAML::Node fileNode, std::ostream& lerr, Mob* mob) {
    EncLevelEnc ele;
    ele.mob = mob;
    ele.weight = readYAMLInt(fileNode, "Freq", 1, "Frequency expected '" + mob->name + "'", lerr);
    YAML::Node oof = fileNode["Group"];
    if (oof.IsSequence()) {
        ele.mobMod.min = oof[0].as<int>();
        ele.mobMod.max = oof[1].as<int>();
    } else if (oof.IsDefined()) {
        ele.mobMod.min = ele.mobMod.max = oof.as<int>();
    } else {
        ele.mobMod.min = ele.mobMod.max = 1;
    }
    if (fileNode["Equip"]) {
        int eiet = -2;
        String equipStr = fileNode["Equip"].as<String>();
        if (equipStr[equipStr.size() - 2] == '_') {
            char c = equipStr[equipStr.size() - 1];
            if (c == '?') eiet = -1;
            else if (c == '!') eiet = -2;
            else eiet = cti(c);
            equipStr = equipStr.substr(0, equipStr.size() - 2);
        }
        if (MobEquipSet::has(equipStr)) {
            ele.mobMod.mobEquipSet = MobEquipSet::get(equipStr);
            ele.mobMod.equipsInEquipsType = eiet;
        }
    } else {
        ele.mobMod.mobEquipSet = NULL;
        ele.mobMod.equipsInEquipsType = -3;
    }
    ele.mobMod.dispersion = 0;
    return ele;
}

void MobSpawner::parseSpawn(YAML::Node fileNode, std::ostream& lerr) {
    int spawnI = addEnvironment(readYAMLStr(fileNode, "Name", "nil", "Spawn enviro name expected.", lerr));
    if (fileNode["Mobs"]) {
        YAML::Node mobsNode = fileNode["Mobs"];
        for (YAML::const_iterator iter = mobsNode.begin(); iter != mobsNode.end(); ++iter) {
            String key = iter->first.as<String>();
            int from, to;
            if (isPair(key)) {
                std::pair<int, int> p = stp(key);
                from = p.first;
                to = p.second;
            } else {
                from = to = sti(key);
            }
            YAML::Node levNode = iter->second;
            for (int i = from; i <= to; i++) {
                EncounterLevel* encounters = new EncounterLevel;
                for (YAML::const_iterator jter = levNode.begin(); jter != levNode.end(); ++jter) {
                    String mobName = jter->first.as<String>();
                    if (Unit::mobExists(mobName)) {
                        if (jter->second.IsSequence()) {
                            for (YAML::const_iterator kter = jter->second.begin(); kter != jter->second.end(); ++kter) {
                                YAML::Node n = *kter;
                                encounters->push_back(parseSpawnMob(n, lerr, Unit::getMob(mobName)));
                            }
                        } else {
                            encounters->push_back(parseSpawnMob(jter->second, lerr, Unit::getMob(mobName)));
                        }
                    } else {
                        lerr << "'" << mobName << "' is not an existing unit.\n";
                    }
                }
                addEncounters(spawnI, i, encounters);
            }
        }
    }
    if (fileNode["Items"]) {
        YAML::Node itemsNode = fileNode["Items"];
        for (YAML::const_iterator iter = itemsNode.begin(); iter != itemsNode.end(); ++iter) {
            String name = iter->first.as<String>();
            if (itemSpawnSetNameMap.find(name) != itemSpawnSetNameMap.end()) {
                int from = iter->second[0].as<int>();
                int to = iter->second[1].as<int>();
                for (int i = from; i <= to; i++) {
                    addItemsToEncounterLevel(spawnI, i, name);
                }
            } else {
                lerr << "'" << name << "' is not an existing item spawn set.\n";
            }
        }
    }
    if (fileNode["Blobs"]) {
        YAML::Node blobNode = fileNode["Blobs"];
        for (YAML::const_iterator iter = blobNode.begin(); iter != blobNode.end(); ++iter) {
            YAML::Node curNode = iter->second;
            BlobGen* blob = new BlobGen;
            String name = iter->first.as<String>();
            if (Unit::mobExists(name)) {
                blob->mob = Unit::getMob(name);
            } else lerr << "'" << name << "' is not an exsiting mob.\n";
            String shape = readYAMLStr(curNode, "Shape", "", "Shape expected.", lerr);
            if (shape == "circle") blob->shape = BLOB_CIRCLE;
            else blob->shape = BLOB_NONE;
            if (curNode["Amount"].IsSequence()) {
                Coord c = readYAMLCoord(curNode, "Amount", Coord(1, 1));
                blob->minAmount = c.x;
                blob->maxAmount = c.y;
            } else blob->minAmount = blob->maxAmount = readYAMLInt(curNode, "Amount", 1);
            if (curNode["Size"].IsSequence()) {
                Coord c = readYAMLCoord(curNode, "Size", Coord(1, 1));
                blob->minSize = c.x;
                blob->maxSize = c.y;
            } else blob->minSize = blob->maxSize = readYAMLInt(curNode, "Size", 1);
            spawnings[spawnI].blobgens.insert(blob);
        }
    }
}
