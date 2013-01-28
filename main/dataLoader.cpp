/* THE DATA LOADER LOADS ALL THE DATA YES. It reads the data files and loads the textures and fonts. */

#include "Start.h"
#include "dataLoader.h"

int getStructIntFromChar(int c) {
    if (c < 50) return (c - 38) / 2 + 26;
    else        return c - 97;
}

int stringToStatus(string line) {
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
    } else if (line == "MAP") {
        return MAP;
    } else if (line == "AREA") {
        return AREA;
    } else if (line == "MONSTER") {
        return MONSTER;
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

    for (map<string, vector<Zone*>*>::iterator i = areaZones.begin(); i != areaZones.end(); i++) {
        for (vector<Zone*>::iterator j = i->second->begin(); j != i->second->end(); j++) {
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

    tileGroups.clear();
    areaZones.clear();
    itemTypeMap.clear();
    itemsToEquip.clear();
    statMap.clear();
    conditionMap.clear();
}

void Start::openFile(string fileName, World* w, Player* p) {
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
            case MTG:
                tempMap = new map<char, string>;
                tileGroups[line] = tempMap;
                break;
            case (MTG + 1):
                (*tempMap)[line[0]] = line.substr(2, 100);
                continue;
            case TILE: tempStr = line; break;
            case (TILE + 1): {
                int c = 0;
                while (line[c] != ':') c++;
                pair<int, int> nums = stp(line.substr(0, c));
                tempValues[1] = sti(line.substr(c + 1, 100));
                tempValues[0] = nums.first + nums.second * (getTexture(tempValues[1])->width / TILE_SIZE);
                } break;
            case (TILE + 2): {
                pair<int, int> nums = stp(line);
                tiles[tempStr] = new Tile(tempValues[0], tempValues[1], nums.first, nums.second);
                } break;
            case (TILE + 3):
                if (line[0] == '*') {
                    bool blockM = line[1] == 't';
                    bool blockL = line[2] == 't';
                    tiles[tempStr]->setBlock(blockM, blockL);
                } else {
                    tiles[tempStr]->setOver(tiles[line]);
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
                    map<char, string>* temp = tileGroups[s];
                    for (map<char, string>::iterator i = temp->begin(); i != temp->end(); i++) {
                        for (int j = 0; j < tempDun->getDepth(); j++) {
                            tempDun->getZone(j)->addTile(tiles[i->second]);
                        }
                    }
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
                        while (line[c] != ',' && c < line.size()) c++;
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
            case MAP:
                tempStr = line;
                tempValues[3] = 0;
                tempMap = new map<char, string>;
                break;
            case (MAP + 1):
                switch(line[0]) {
                    case '>': {
                        string s = line.substr(1, 100);
                        if (ERRCHECK && areas.find(s) == areas.end()) printFileProb("This is not an existing area.", lineNum);
                        tempStr2 = s;
                        } continue;
                    case 'v': tempValues[2] = sti(line.substr(1, 100)); continue;
                    case ':': {
                        string s = line.substr(1, 100);
                        if (ERRCHECK && tileGroups.find(s) == tileGroups.end()) printFileProb("This is not an existing tile group.", lineNum);
                        map<char, string>* temp = tileGroups[line.substr(1, 100)];
                        tempMap->insert(temp->begin(),temp->end());
                        } continue;
                    case '[':
                        if (line[1] == '[') {
                            status = MAP + 10;
                            continue;
                        } else {
                            tempVect.clear();
                        } break;
                    case '*': {
                        string s = line.substr(1, 100);
                        if (ERRCHECK && !isNum(s)) printFileErr("Light needs to be an integer.", lineNum);
                        int num = sti(s);
                        if (ERRCHECK && num <= 0) printFileErr("Light level must be positive.", lineNum);
                        tempValues[0] = sti(s);
                        } continue;
                    default:
                        if (line[1] == '-') {
                            (*tempMap)[line[0]] = line.substr(2, 100);
                        } else if (ERRCHECK) {
                            printFileErr("This line has an unknown goal.", lineNum);
                        }
                        continue;
                } break;
            case (MAP + 2):
                if (line[0] == ']') {
                    if (ERRCHECK && tempVect.empty()) printFileErr("Where is the map?", lineNum);
                    else {
                        status = MAP + 3;
                        int wid = tempVect[0].size() / 4 + 1;
                        if (ERRCHECK) {
                            for (unsigned int i = 0; i < tempVect.size(); i++) {
                                if (tempVect[i].size() / 4 + 1 != (unsigned int)wid) printFileErr("Inconsistant row widths in this map.", lineNum);
                            }
                        }
                        int hei = tempVect.size();
                        Zone* newZone = new Zone(tempStr, wid, hei, tempValues[0], true);
                        int* heights = new int[wid * hei];
                        int* ntiles = new int[wid * hei];
                        int* structs = new int[wid * hei];
                        for (int i = 0; i < wid; i++) {
                            for (int j = 0; j < hei; j++) {
                                int t = tempVect.at(j).at(i * 4);
                                int b = tempVect.at(j).at(i * 4 + 1);
                                if (b < 60)       b -= 48;
                                else if (b < 90) b -= 55;
                                else              b -= 87;
                                int sc = getStructIntFromChar(tempVect.at(j).at(i * 4 + 2));
                                if (ERRCHECK) {
                                    if (tempMap->find(t) == tempMap->end()) printFileProb("Invalid tile used somewhere in this map.", lineNum);
                                    if (b < 0 || b >= 32) printFileProb("Invalid height used somewhere in this map.", lineNum);
                                    if (sc < 0 || sc >= 32) printFileProb("Invalid struct used somewhere in this map.", lineNum);
                                }
                                int k = i + j * wid;
                                ntiles[k] = distance(tempMap->begin(), tempMap->find(t));
                                heights[k] = b;
                                structs[k] = sc;
                            }
                        }
                        newZone->fillHeights(heights);
                        newZone->fillTiles(ntiles);
                        newZone->fillStructs(structs);
                        for (map<char, string>::iterator i = tempMap->begin(); i != tempMap->end(); i++) {
                            newZone->addTile(tiles[i->second]);
                        }
                        areaZones[tempStr2]->push_back(newZone);
                        zones[tempStr] = newZone;
                        tempZone = newZone;
                        delete tempMap;
                    }
                } else {
                    tempVect.push_back(line);
                } continue;
            case (MAP + 3):
                switch(line[0]) {
                    case '$': //item
                        tempStr = line.substr(1, 100);
                        status = MAP + 4;
                        break;
                    case '@': //unit
                        tempStr = line.substr(1, 100);
                        status = MAP + 5;
                        break;
                    case '^': //height
                        tempValues[0] = sti(line.substr(1, 100));
                        status = MAP + 6;
                        break;
                    case '<': //struct
                        tempChar = line[1];
                        status = MAP + 7;
                        break;
                    default: break;
                } continue;
            case (MAP + 4): {
                pair<int, int> nums = stp(line);
                Item newItem;
                newItem.itemType = itemTypeMap[tempStr];
                addItemToPlace(nums.first, nums.second, tempZone, newItem);
                status = MAP + 3;
                } continue;
            case (MAP + 5): {
                pair<int, int> nums = stp(line);
                Unit* newUnit = new Unit(tempStr, mobSpawner->getMob(tempStr).second);
                newUnit->x = nums.first;
                newUnit->y = nums.second;
                tempZone->getLocationAt(nums.first, nums.second)->unit = newUnit;
                status = MAP + 3;
                } continue;
            case (MAP + 6): {
                pair<int, int> nums = stp(line);
                tempZone->getLocationAt(nums.first, nums.second)->height = tempValues[0];
                status = MAP + 3;
                } continue;
            case (MAP + 7): {
                pair<int, int> nums = stp(line);
                tempZone->getLocationAt(nums.first, nums.second)->structure = getStructIntFromChar(tempChar);
                status = MAP + 3;
                } continue;
            case (MAP + 10): {
                pair<int, int> nums = stp(line);
                tempZone = new Zone(tempStr, nums.first, nums.second, tempValues[0], false);
                } break;
            case PLAYER: p->setName(line); break;
            case (PLAYER + 1): {
                mob unitProto = mobSpawner->getMob(line);
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
                pair<int, int> nums = stp(line);
                p->getUnit()->x = nums.first;
                p->getUnit()->y = nums.second;
                p->getZone()->getLocationAt(p->getUnit()->x, p->getUnit()->y)->unit = p->getUnit();
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
            case (STAIRS + 2): {
                pair<int, int> nums = stp(line);
                tempValues[0] = nums.first;
                tempValues[1] = nums.second;
                } break;
            case (STAIRS + 3): {
                map<string, Zone*>::iterator zi = zones.find(line);
                if (zi == zones.end()) {
                    map<string, DungeonStack*>::iterator di = dungeons.find(line);
                    if (ERRCHECK && di == dungeons.end()) printFileProb("No zone or dungeon with that name has been defined.", lineNum);
                    DungeonStack* dungeonStack = di->second;
                    pair<short, short> coords = dungeonStack->addEntryStairs();
                    tempArea->addConnection(connection{tempValues[0], tempValues[1], coords.first, coords.second, tempZone, dungeonStack->getZone(0)});
                    status = STAIRS + 5;
                } else {
                    tempZone2 = zi->second;
                } } break;
            case (STAIRS + 4): {
                connection c;
                c.x1 = tempValues[0];
                c.y1 = tempValues[1];
                c.z1 = tempZone;
                c.z2 = tempZone2;
                pair<int, int> nums = stp(line);
                c.x2 = nums.first;
                c.y2 = nums.second;
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
                    int num = sti(line.substr(i + 1, 100));
                    if (getStat(V_UNIT, stat)->isItFloat()) {
                        unitPrototype->addStatVF(stat, num);
                    } else {
                        unitPrototype->addStatV(stat, num);
                    }
                    if (s == "split") {
                        status = UNIT + 2;
                    }
                }
            } continue;
            case (UNIT + 2):
                mobSpawner->getMob(tempStr).second->addStatV(S_SPAWN, mobSpawner->hashMob(line));
                status = UNIT + 1;
            continue;
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
                    encounterLevel* encounters = new encounterLevel;
                    int level = cti(line[0]);
                    int mon = 0;
                    while ((mon + 1) * 10 < (signed int)line.size()) {
                        string monName = line.substr(mon * 10 + 2, 6);
                        int weight = sti(line.substr(mon * 10 + 9, 2));
                        mob theMob = mobSpawner->getMob(monName);
                        if (ERRCHECK && theMob.first == "x") printFileProb("This mob does not exist.", lineNum);
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
                            int index = 0;
                            MobMod* relevent = &(encounters->at(encounters->size() - 1).mobMod);
                            relevent->min = min; relevent->max = max;
                            relevent->dispersion = disp;
                            if (line[mon * 10 + 6] != '-') {
                                MobEquipSet* ridiculous = mobEquipsMap[line.substr(mon * 10 + 6, 3)];
                                c = line[mon * 10 + 9];
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
                    Tile* theTile = tiles[line.substr(c + 2, 100)];
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
                    while (line[c] != '(' && c < line.size()) c++;
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
                        while (line[c] != ',' && c < line.size()) c++;
                        string s = line.substr(d, c - d);
                        mobEquipSet->addEquipToEquips(v, itemTypeMap[s]);
                        c += 2;
                        d = c;
                    }
            } } continue;
            default: status = BETWEEN; break;
        }
        status++;
    }
}

void Start::deleteData() {
    tiles.clear();
    areas.clear();
    zones.clear();
    clearTextures();
    clearItemTypes();
    glDeleteLists(base, 96);
}
