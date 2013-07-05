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

/* THE DATA LOADER LOADS ALL THE DATA YES. It reads the data files and loads the textures and fonts. */

#include "Start.h"
#include "dataLoader.h"

int getStructIntFromChar(int c) {
    if (c < 50) return (c - 38) / 2 + 26;
    else        return c - 97;
}

int stringToStatus(std::string line) {
    if (line == "TILE") {
        return TILE;
    } else if (line == "MTG") {
        return MTG;
    } else if (line == "WORLD") {
        return WORLD;
    } else if (line == "PLAYER") {
        return PLAYER;
    } else if (line == "FILES") {
        return FILES;
    } else if (line == "AREA") {
        return AREA;
    } else if (line == "SPELLS") {
        return SPELLS;
    } else if (line == "RESOURCES") {
        return RESOURCES;
    } else if (line == "STAIRS") {
        return STAIRS;
    } else if (line == "MAPSPACE") {
        return MAPSPACE;
    } else if (line == "ITEM") {
        return ITEM;
    } else if (line == "STATS") {
        return STATS;
    } else if (line  == "UNIT") {
        return UNIT;
    } else if (line == "MAPSTACK") {
        return MAPSTACK;
    }else if (line == "CONDITIONS") {
        return CONDITIONS;
    } else if (line == "MOBSPAW") {
        return MOBSPAW;
    } else if (line == "ITEMSPAW") {
        return ITEMSPAW;
    } else if (line == "TILEDMAPS") {
        return TILEDMAPS;
    } else if (line == "TILEDMAPSREFER") {
        return TILEDMAPSREFER;
    } else if (line == "SKILLS") {
        return SKILLS;
    } else if (line == "MOBEQUIPS") {
        return MOBEQUIPS;
    } else {
        return -1;
    }
}

/* Loads ALL the data. */
void Start::loadData(World* w, Player* p) {
    using namespace std;

    cout << "Reading data files..." << endl;
    openFile(FIRST_FILE, w, p);
    cout << "Finished reading data files." << endl;

    if (!gotsStructureTex) {
        cout << "WARNING: No structure.png was loaded!" << endl;
    } if (!gotsMenuTex) {
        cout << "WARNING: No menu.png was loaded!" << endl;
    } if (!gotsFontTex) {
        cout << "WARNING: No font.png was loaded!" << endl;
    } if (!gotsSplatterTex) {
        cout << "WARNING: No splatters.png was loaded!" << endl;
    } if (!gotsPlayerTex) {
        cout << "WARNING: No player.png was loaded!" << endl;
    } if (!gotsAttackAnimsTex) {
        cout << "WORNING: No attackAnims.png was loaded!" << endl;
    }

    for (map<string, vector<Zone*>*>::iterator i = areaZones.begin(); i != areaZones.end(); ++i) {
        for (vector<Zone*>::iterator j = i->second->begin(); j != i->second->end(); ++j) {
            areas[i->first]->addZone(*j);
        }
    }

    spawnPrototypes.clear();
    buildFont();
    cout << "All done with data loading." << endl;
}

void Start::finishDataSetup() {
    for (unsigned int i = 0; i < itemsToEquip.size(); i++) {
        equipItem(itemsToEquip[i]);
    }
    for (std::map<std::string, std::vector<Zone*>*>::iterator i = areaZones.begin(); i != areaZones.end(); ++i) {
        i->second->clear();
        delete i->second;
    }
    areaZones.clear();
    itemTypeMap.clear();
    itemsToEquip.clear();
    spellMap.clear();
    statMap.clear();
    skillMap.clear();
    conditionMap.clear();
    tileMap.clear();

    tempStr.clear();
    tempStr2.clear();
}

void Start::openFile(std::string fileName, World* w, Player* p) {
    using namespace std;

    cout << " -Opening " << fileName << "..." << endl;

    if (ERRCHECK) {
        if (!errorChecker(fileName)) return;
    }

    ifstream fin;
    fin.open(fileName.c_str(), ios::in);

    bool comment = false;
    int lineNum = 0;
    string line = "";
    int status = BETWEEN; // The status determine what the loader is looking for next. BETWEEN means it has no particular goal at the moment.
    while(getline(fin, line)) {
        lineNum++;
        //cout << "aou" << lineNum << endl;
        //if this line is empty of a comment, it skips to the next line

        boost::algorithm::trim(line);
        if (line.length() == 0) continue;
        if (comment) {
            if (line[0] == 'x' && line[1] == '~') comment = false;
            continue;
        }
        if (line[0] == '~') {
            if (line.size() >= 2 && line[1] == '~') comment = true;
            continue;
        }
        if (line == "END") {
            status = BETWEEN; //if this line is the end, it skips to the next line and reverts to the main status
            continue;
        }
        if (DEBUG) {
            cout << "    -- " << status << ". " << line << endl;
        }
        //continue is used when I don't want the status to increment at the end
        switch(status) {
            case BETWEEN:
                status = stringToStatus(line);
                continue;
            case MTG: tempStr = line; break;
            case (MTG + 1): tileGroups[tempStr].push_back(tileMap[line]); continue;
            case TILE: tempStr = line; break;
            case (TILE + 1): {
                int c = 0;
                while (line[c] != ':') c++;
                pair<int, int> nums = stp(line.substr(0, c));
                tempValues[1] = sti(line.substr(c + 1, 100));
                tempValues[0] = nums.first + nums.second * (getTexture(tempValues[1])->getWidth() / TILE_SIZE);
                } break;
            case (TILE + 2): {
                pair<int, int> nums = stp(line);
                Tile* newTile = new Tile(tempValues[0], tempValues[1], nums.first, nums.second);
                tileMap[tempStr] = newTile;
                addTile(newTile);
                } break;
            case (TILE + 3):
                if (line[0] == '*') {
                    bool blockM = line[1] == 't';
                    bool blockL = line[2] == 't';
                    tileMap[tempStr]->setBlock(blockM, blockL);
                } else {
                    tileMap[tempStr]->setOver(tileMap[line]);
                } break;
            case WORLD: tempValues[0] = sti(line); break;
            case (WORLD + 1):
                w->setDim(tempValues[0], sti(line));
                break;
            case FILES: {
                string fName = "data/" + line + ".rag";
                openFile(fName, w, p);
                } continue;
            case AREA: tempStr = line; break;
            case (AREA + 1): {
                Area* newArea = new Area(sti(line));
                areas[tempStr] = newArea;
                areaZones[tempStr] = new vector<Zone*>;
                w->addArea(newArea);
                tempValues[0] = sti(line);
                } break;
            case MAPSTACK: {
                int c = 0;
                while (line[c] != ' ') c++;
                string name = line.substr(0, c);
                pair<int, int> nums = stp(line.substr(c + 1, 100));
                DungeonStack* dungeonStack = new DungeonStack(name, nums.first, (GenType)nums.second);
                dungeons[name] = dungeonStack;
                tempDun = dungeonStack;
                } break;
            case (MAPSTACK + 1): {
                tempArea = areas[line.substr(1, 100)];
                } break;
            case (MAPSTACK + 2): {
                string typ = line.substr(0, 3);
                unsigned int b = 3;
                while (line[b] != ' ') b++;
                b++;
                unsigned int c = 0;
                if (line[0] == ':') {
                    string s = line.substr(1, 100);
                    if (ERRCHECK && tileGroups.find(s) == tileGroups.end()) printFileProb("That tile group simply does not exist.", lineNum);
                    tempDun->setTileset(&tileGroups[s][0]);
                } else if (typ == "lig") {
                    while (c + b < line.size() && line[c + b] != '+') c++;
                    int light = sti(line.substr(b, c));
                    float change = 0.f;
                    if (c + b < line.size()) {
                        b += c + 1; c = 0;
                        while (c + b < line.size()) c++;
                        change = stf(line.substr(b, c));
                    }
                    tempDun->setLight(light, change);
                } else if (typ == "spa") {
                    vector<short> environments;
                    int i = 0;
                    while(line[c] != ' ') c++;
                    int d = ++c;
                    while (c < line.size()) {
                        while (c < line.size() && line[c] != ',') c++;
                        environments.push_back(sti(line.substr(d, c - d)));
                        d = ++c; i++;
                    }
                    tempDun->setEnvironments(environments.size(), &environments[0]);
                } else if (typ == "lev") {
                    while (c + b < line.size() && line[c + b] != '+') c++;
                    int level = sti(line.substr(b, c));
                    float change = 0.f;
                    if (c + b < line.size()) {
                        b += c + 1; c = 0;
                        while (c + b < line.size()) c++;
                        change = stf(line.substr(b, c));
                    }
                    tempDun->setDifficulty(level, change);
                } else if (typ == "dim") {
                    while (line[c + b] != ',' && line[c + b] != '+') c++;
                    int width = sti(line.substr(b, c));
                    float wChange = 0.f;
                    if (line[c + b] == '+') {
                        b += c + 1; c = 0;
                        while (line[c + b] != ',') c++;
                        wChange = stf(line.substr(b, c));
                    }
                    b += c + 1; c = 0;
                    while (c + b < line.size() && line[c + b] != '+') c++;
                    int height = sti(line.substr(b, c));
                    float hChange = 0.f;
                    if (c + b < line.size()) {
                        b += c + 1; c = 0;
                        while (c + b < line.size()) c++;
                        hChange = stf(line.substr(b, c));
                    }
                    tempDun->setDimensions(width, height, wChange, hChange);
                } else if (typ == "sta") {
                    while (c + b < line.size() && line[c + b] != '+') c++;
                    int stairs = sti(line.substr(b, c));
                    float change = 0.f;
                    if (c + b < line.size()) {
                        b += c + 1; c = 0;
                        while (c + b < line.size()) c++;
                        change = stf(line.substr(b, c));
                    }
                    tempDun->setNumStairs(stairs, change);
                } else if (typ == "!!!") {
                    tempDun->createZones();
                    tempArea->addDungeonStack(tempDun);
                } else if (ERRCHECK) printFileProb("That's not an option!", lineNum);
            } continue;
            case PLAYER: p->setName(line); break;
            case (PLAYER + 1): {
                Mob unitProto = mobSpawner->getMob(line);
                if (ERRCHECK && unitProto.first == "x") printFileProb("That unit does not seem to exist.", lineNum);
                p->setUnitProto(unitProto.second);
                } break;
            case (PLAYER + 2): {
                if (ERRCHECK && areas.find(line) == areas.end()) printFileProb("That area does not seem to exist.", lineNum);
                p->setArea(areas[line]);
                } break;
            case (PLAYER + 3): {
                if (ERRCHECK && zones.find(line) == zones.end()) printFileProb("That zone does not seem to exist.", lineNum);
                p->setZone(zones[line]);
                } break;
            case (PLAYER + 4): {
                p->getUnit()->pos = stc(line);
                p->getZone()->getLocationAt(p->getUnit()->pos)->unit = p->getUnit();
                } break;
            case (PLAYER + 6):
                if (line[0] == '-') {
                } else {
                    if (ERRCHECK && itemTypeMap.find(line) == itemTypeMap.end()) printFileProb("That item does not seem to exist.", lineNum);
                    itemsToEquip.push_back(Item(itemTypeMap[line]));
                    continue;
                }
            case (PLAYER + 5):
                switch(line[1]) {
                    case 'e': status = PLAYER + 6; continue;
                    default: continue;
                } break;
            case RESOURCES: loadImage(line); continue;
            case MAPSPACE:
                if (ERRCHECK && areaZones.find(line) == areaZones.end()) printFileProb("Nope!", lineNum);
                areaZones[line]->push_back(NULL); break;
            case STAIRS:
                if (ERRCHECK && areas.find(line) == areas.end()) printFileProb("No area with that name has been defined.", lineNum);
                tempArea = areas[line]; break;
            case (STAIRS + 1):
                if (ERRCHECK && zones.find(line) == zones.end()) printFileProb("No zone with that name has been defined.", lineNum);
                tempZone = zones[line]; break;
            case (STAIRS + 2): tempCoord = stc(line); break;
            case (STAIRS + 3): {
                map<string, Zone*>::iterator zi = zones.find(line);
                if (zi == zones.end()) {
                    map<string, DungeonStack*>::iterator di = dungeons.find(line);
                    if (ERRCHECK && di == dungeons.end()) printFileProb("No zone or dungeon with that name has been defined.", lineNum);
                    DungeonStack* dungeonStack = di->second;
                    Coord coord = dungeonStack->addEntryStairs();
                    tempArea->addConnection(Connection{tempCoord, coord, tempZone, dungeonStack->getZone(0)});
                    status = STAIRS + 5;
                } else {
                    tempZone2 = zi->second;
                } } break;
            case (STAIRS + 4): {
                Connection c;
                c.loc1 = tempCoord;
                c.z1 = tempZone;
                c.z2 = tempZone2;
                c.loc2 = stc(line);
                tempArea->addConnection(c);
                status = STAIRS + 1;
                } continue;
            case ITEM: tempStr = line; break;
            case (ITEM + 1): tempStr2 = line; break;
            case (ITEM + 2): tempValues[0] = sti(line); break;
            case (ITEM + 3): tempValues[1]= sti(line); break;
            case (ITEM + 4): {
                ItemType* newItem = new ItemType(tempStr, tempStr2, tempValues[0], tempValues[1], sti(line));
                itemTypeMap[tempStr] = addItemType(newItem);
                } break;
            case (ITEM + 5): {
                ItemType* itemType = getItemType(itemTypeMap[tempStr]);
                if (line[0] == '*') {
                    unsigned int i = 1;
                    while(line[i] != ',') i++;
                    int num = sti(line.substr(1, i - 1));
                    string s = line.substr(i + 1, 100);
                    EquipGType t = EQG_NONE;
                    if (s == "NORM") t = EQG_NORM;
                    else if (s == "LONG") t = EQG_LONG;
                    else if (s == "TALL") t = EQG_TALL;
                    else if (s == "SMALL") t = EQG_SMALL;
                    else if (s == "GNORM") t = EQG_GNORM;
                    else if (s == "GLONG") t = EQG_GLONG;
                    else if (s == "GTALL") t = EQG_GTALL;
                    else if (s == "GSMALL") t = EQG_GSMALL;
                    else printFileProb("That is not an avaliable item equip graphic shape type form condition.", lineNum);
                    itemType->setEquipGraphic(t, num);
                } else if (line[0] == '(') {
                    unsigned int c = 1;
                    unsigned int d = c;
                    while (c < line.size()) {
                        if (line[c] == ',' || line[c] == ')') {
                            string s = line.substr(d, c - d);
                            itemType->addAbility(spellMap[s]);
                            c++;
                            d = c;
                        }
                        c++;
                    }
                } else {
                    int i = 0;
                    while(line[i] != '=') {
                        i++;
                    }
                    string s = line.substr(0, i);
                    if (ERRCHECK && statMap.find(s) == statMap.end()) printFileProb("That is not an existing stat >:(.", lineNum);
                    int stat = statMap[s];
                    int num = sti(line.substr(i + 1, 100));
                    itemType->addStatV(stat, num);
                }}  continue;
            case (STATS + 2):
                if (line[0] != '-') {
                    if (ERRCHECK && statMap.find(line) == statMap.end()) printFileProb("That is not an existing stat >:O.", lineNum);
                    defaultStats.push_back(statMap[line]);
                    continue;
                }
            case STATS:
                if (line[0] == '-') {
                    string s = line.substr(1, 100);
                    if (s == "unit") {
                        tempValues[0] = V_UNIT;
                    } else if (s == "item") {
                        tempValues[0] = V_ITEM;
                    } else if (ERRCHECK) {
                        printFileErr("Not an option.", lineNum);
                    }
                } else if (line[0] == '$') {
                    status = STATS + 2;
                } else {
                    tempStr = line;
                    status = STATS + 1;
                } continue;
            case (STATS + 1): {
                parseFormula(line, ERRCHECK, lineNum);
                status = STATS;
            } continue;
            case UNIT: {
                pair<string, string> strs = stps(line);
                tempStr = strs.second;
                StatHolder* newUnit = new StatHolder(V_UNIT);
                mobSpawner->addMob(strs.first, strs.second, newUnit);
            } break;
            case (UNIT + 1): {
                StatHolder* unitPrototype = mobSpawner->getMob(tempStr).second;
                if (line[0] == '*') {
                    for (unsigned int i = 0; i < defaultStats.size(); i++) {
                        int theStatI = defaultStats[i];
                        Stat* theStat = getStat(V_UNIT, theStatI);
                        if (!unitPrototype->hasStat(theStatI, theStat->isItFloat())) {
                            if (theStat->isItFloat()) {
                                unitPrototype->addStatF(theStatI);
                            } else {
                                unitPrototype->addStat(theStatI);
                            }
                        }
                    }
                } else {
                    int i = 0;
                    while(line[i] != '=') {
                        i++;
                    }
                    string s = line.substr(0, i);
                    if (ERRCHECK && statMap.find(s) == statMap.end()) printFileProb("That is not an existing stat >:C.", lineNum);
                    int stat = statMap[s];
                    string st = line.substr(i + 1, 100);
                    if (isNum(st)) {
                        int num = sti(st);
                        if (getStat(V_UNIT, stat)->isItFloat()) {
                            unitPrototype->addStatVF(stat, num);
                        } else {
                            unitPrototype->addStatV(stat, num);
                        }
                    } else {
                        //cout << "this better effing WORK {" << st << "} URRRGM" << mobSpawner->hashMob(st) << endl;
                        unitPrototype->addStatV(stat, mobSpawner->hashMob(st));
                    }
                }
            } continue;
            case CONDITIONS:
                tempValues[0] = 0;
                conditionMap[line] = tempValues[0];
                break;
            case (CONDITIONS + 1):
                tempValues[0]++;
                conditionMap[line] = tempValues[0];
                continue;
            case MOBSPAW: tempValues[0] = mobSpawner->addEnvironment(line.substr(0, line.size() - 1)); break;
            case (MOBSPAW + 1): break;
            case (MOBSPAW + 2): {
                if (line[0] == ']') {
                    if (line[1] == 'i') status = MOBSPAW + 3;
                    else status = MOBSPAW;
                } else {
                    EncounterLevel* encounters = new EncounterLevel;
                    int level = cti(line[0]);
                    int mon = 0;
                    while ((mon + 1) * 10 < (signed int)line.size()) {
                        string monName = line.substr(mon * 10 + 2, 6);
                        unsigned int weight = sti(line.substr(mon * 10 + 9, 2));
                        Mob mob = mobSpawner->getMob(monName);
                        if (ERRCHECK && mob.first == "x") printFileProb("This mob does not exist.", lineNum);
                        encounters->push_back(EncLevelEnc{mobSpawner->getMob(monName), weight, MobMod{1, 1, -3, 0, NULL}});
                        mon++;
                        if (line[mon * 10 + 1] == '(') {
                            int min = line[mon * 10 + 2] - 48;
                            if (min > 10) min -= 7;
                            int max = line[mon * 10 + 4] - 48;
                            if (max > 10) max -= 7;
                            int disp = 0;
                            char c = line[mon * 10 + 3];
                            if (c == '_') disp = 0;
                            else if (c == '-') disp = 1;
                            else if (c == '~') disp = 2;
                            MobMod* relevent = &(encounters->at(encounters->size() - 1).mobMod);
                            relevent->min = min; relevent->max = max;
                            relevent->dispersion = disp;
                            if (line[mon * 10 + 6] != '-') {
                                MobEquipSet* ridiculous = mobEquipsMap[line.substr(mon * 10 + 6, 3)];
                                c = line[mon * 10 + 9];
                                int index = 0;
                                if (c == '!') index = -2;
                                else if (c == '?') index = -1;
                                else index = cti(c);
                                if (index > 10) index -= 7;
                                relevent->mobEquipSet = ridiculous;
                                relevent->equipsInEquipsType = index;
                            }
                            mon++;
                        }
                    }
                    mobSpawner->addEncounters(tempValues[0], level, encounters);
            } } continue;
            case (MOBSPAW + 3): {
                if (line[0] == ']') {
                    status = MOBSPAW;
                } else {
                    int c = 0;
                    while (line[c] != ':') c++;
                    string s = line.substr(0, c);
                    pair<int, int> nums = stp(line.substr(c + 1, 100));
                    for (int i = nums.first; i <= nums.second; i++) {
                        mobSpawner->addItemsToEncounterLevel(tempValues[0], i, s);
                    }
                } continue;
            }
            case ITEMSPAW: tempValues[0] = mobSpawner->addItemSpawnSet(line.substr(0, line.size() - 1)); break;
            case (ITEMSPAW + 1): {
                if (line[0] == '}') {
                    status = ITEMSPAW;
                } else {
                    int minS = 1;
                    int maxS = 1;
                    int c = 0;
                    bool done = false;
                    string s;
                    while (!done) {
                        if (line[c] == ':') {
                            s = line.substr(0, c);
                            done = true;
                        } else if (line[c] == '(') {
                            s = line.substr(0, c);
                            int d = c + 1;
                            while(!done) {
                                if (line[c] == ')') {
                                    minS = sti(line.substr(d, c - d));
                                    maxS = minS;
                                    done = true;
                                } else if (line[c] == '-') {
                                    minS = sti(line.substr(d, c - d));
                                    d = c + 1;
                                    while(line[c] != ')') c++;
                                    maxS = sti(line.substr(d, c - d));
                                    done = true;
                                }
                                c++;
                            }
                        }
                        c++;
                    }
                    string s2 = line.substr(c, 100);
                    int weight = 0;
                    if (s2 == "eeverywhere") {
                        weight = 10000;
                    } else if (s2 == "everywhere") {
                        weight = 3500;
                    } else if (s2 == "very common") {
                        weight = 1000;
                    } else if (s2 == "common") {
                        weight = 350;
                    } else if (s2 == "uncommon") {
                        weight = 100;
                    } else if (s2 == "rare") {
                        weight = 35;
                    } else if (s2 == "very rare") {
                        weight = 10;
                    } else if (s2 == "extremely rare") {
                        weight = 3;
                    } else if (s2 == "rrare") {
                        weight = 1;
                    } else if (s2 == "nonexistant") {
                        weight = 0;
                    } else {
                        weight = sti(s2);
                    }
                    if (ERRCHECK && itemTypeMap.find(s) == itemTypeMap.end()) printFileProb("This item does not exist.", lineNum);
                    int itemTypeI = itemTypeMap[s];
                    mobSpawner->addItemToSpawnSet(itemTypeI, weight, minS, maxS, tempValues[0]);
                }
            } continue;
            case TILEDMAPS: tempStr = line.substr(1, 100); break;
            case (TILEDMAPS + 1): tempStr2 = line; break;
            case (TILEDMAPS + 2): {
                Zone* z = loadTileFile("resources/tilemaps/" + line, tempStr2);
                areaZones[tempStr]->push_back(z);
                zones[tempStr2] = z;
                status = TILEDMAPS + 1;
            } continue;
            case TILEDMAPSREFER: {
                string s = line.substr(1, 100);
                if (s == "tiles") status = TILEDMAPSREFER + 1;
                else if (s == "items") status = TILEDMAPSREFER + 2;
                else if (s == "mobs") status = TILEDMAPSREFER + 3;
            } continue;
            case (TILEDMAPSREFER + 1):
                if (line[0] == '-') {
                    string s = line.substr(1, 100);
                    if (s == "items") status = TILEDMAPSREFER + 2;
                    else if (s == "mobs") status = TILEDMAPSREFER + 3;
                } else {
                    int c = 0;
                    while (line[c] != '-') c++;
                    pair<int, int> nums = stp(line.substr(0, c));
                    int ind = nums.second * 32 + nums.first;
                    Tile* theTile = tileMap[line.substr(c + 2, 100)];
                    tiledTiles[ind] = theTile;
                } continue;
            case (TILEDMAPSREFER + 2):
                if (line[0] == '-') {
                    string s = line.substr(1, 100);
                    if (s == "mobs") status = TILEDMAPSREFER + 3;
                } else {
                    unsigned int c = 0;
                    while (line[c] != '-') c++;
                    pair<int, int> nums = stp(line.substr(0, c));
                    int ind = nums.second * 32 + nums.first;
                    c += 2; int d = c;
                    while (c < line.size() && line[c] != '(') c++;
                    int theItemI = itemTypeMap[line.substr(d, c - d)];
                    RandItemType* theRandItemType;
                    if (c < line.size()) {
                        c++; d = c;
                        while (line[c] != ')' && line[c] != '-') c++;
                        int num1 = sti(line.substr(d, c - d));
                        if (line[c] == '-') {
                            c++; d = c;
                            while (line[c] != ')') c++;
                            int num2 = sti(line.substr(d, c - d));
                            theRandItemType = new RandItemType(theItemI, num1, num2, 0);
                        } else {
                            theRandItemType = new RandItemType(theItemI, num1, num1, 0);
                        }
                    } else {
                        theRandItemType = new RandItemType(theItemI, 1, 1, 0);
                    }
                    tiledItems[ind] = theRandItemType;
                } continue;
            case (TILEDMAPSREFER + 3): {
                unsigned int c = 0;
                while (line[c] != '-') c++;
                pair<int, int> nums = stp(line.substr(0, c));
                int ind = nums.second * 32 + nums.first;
                int tag = mobSpawner->hashMob(line.substr(c + 2, 100));
                tiledMobs[ind] = tag;
            } continue;
            case SKILLS:
                tempValues[0] = 0;
                status++;
            case (SKILLS + 1): {
                unsigned int c = 0;
                int d = 0;
                while (c < line.size()) {
                    if (line[c] == ',') {
                        skillMap[line.substr(d, c - d)] = tempValues[0];
                        tempValues[0]++;
                        d = -1;
                    } else if (d == -1 && line[c] != ' ') {
                        d = c;
                    }
                    c++;
            } } continue;
            case MOBEQUIPS:
                mobEquipsMap[line] = new MobEquipSet();
                tempStr = line;
                break;
            case (MOBEQUIPS + 1): {
                MobEquipSet* mobEquipSet = mobEquipsMap[tempStr];
                unsigned int c = 0;
                while (line[c] != ':') c++;
                int weight = sti(line.substr(0, c));
                int v = mobEquipSet->addEquips(weight);
                c++;
                if (line[c] != 'X') {
                    int d = c;
                    while (c < line.size()) {
                        while (c < line.size() && line[c] != ',') c++;
                        string s = line.substr(d, c - d);
                        mobEquipSet->addEquipToEquips(v, itemTypeMap[s]);
                        c += 2;
                        d = c;
                    }
            } } continue;
            case SPELLS: tempStr = line; break;
            case (SPELLS + 1): {
                unsigned int c = 0;
                int d = 0;
                bool wind, earth, fire, water, phys, mind, light, dark;
                wind = earth = fire = water = phys = mind = light = dark = false;
                int power = 0;
                while (c < line.size()) {
                    c++;
                    if (c >= line.size() || line[c] == '+') {
                        string s = line.substr(d, 2);
                        if (s == "wi") wind = true;
                        else if (s == "ea") earth = true;
                        else if (s == "fi") fire = true;
                        else if (s == "wa") water = true;
                        else if (s == "ph") phys = true;
                        else if (s == "mi") mind = true;
                        else if (s == "li") light = true;
                        else if (s == "da") dark = true;
                        c++;
                        d = c;
                    } else if (line[c] == '*') {
                        power++;
                        d = c + 1;
                    }
                }
                tempValues[0] = (wind << 7) | (earth << 1) | (water << 3) | (fire << 9) | (phys << 4) | (mind << 6) | (light << 8) | (dark << 2) | power;
            } break;
            case (SPELLS + 2): {
                pair<int, int> nums = stp(line);
                tempValues[1] = nums.first;
                tempValues[2] = nums.second;
            } break;
            case (SPELLS + 3): {
                Ability* newSpell = new Ability(tempStr, tempValues[1], tempValues[2], sti(line), tempValues[0]);
                spellMap[tempStr] = tempValues[0];
                addAbility(newSpell);
                status = SPELLS;
            } continue;
            default: status = BETWEEN; break;
        }
        status++;
    }
}

void Start::deleteData() {
    for (std::map<std::string, MobEquipSet*>::iterator i = mobEquipsMap.begin(); i != mobEquipsMap.end(); ++i) {
        delete i->second;
    }
    mobEquipsMap.clear();
    for (std::map<std::string, Area*>::iterator i = areas.begin(); i != areas.end(); ++i) {
        delete i->second;
    }
    areas.clear();
    zones.clear();
    clearTextures();
    clearItemTypes();
    clearTiles();
    tileGroups.clear();
    glDeleteLists(base, 96);
}
