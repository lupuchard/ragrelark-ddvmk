/* THE DATA LOADER LOADS ALL THE DATA YES. It reads the data files and loads the textures and fonts. */

#include "Start.h"

#define BETWEEN 0
#define MTG 100
#define TILE 200
#define WORLD 300
#define PLAYER 400
#define FILES 500
#define MAP 600
#define MONSTER 700
#define AREA 800
#define RESOURCES 900
#define CONNECTION 1000
#define MAPSPACE 1100
#define ITEM 1200
#define STATS 1300
#define UNIT 1400
#define MAPSTACK 1500
#define FEATURE 1600
#define CONDITIONS 1700
#define MOBSPAW 1800
#define ITEMSPAW 1900

#define FIRST_FILE "data/base.txt"

#define DEBUG false
#define ERRCHECK true

int getStructIntFromChar(int c) {
    if (c < 50) return (c - 38) / 2 + 26;
    else        return c - 97;
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
    }

    for (map<string, vector<Zone*>*>::iterator i = areaZones.begin(); i != areaZones.end(); i++) {
        areas[i->first]->setZones(&(*(i->second))[0], i->second->size());
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
    } else if (line == "CONNECTION") {
        return CONNECTION;
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
    } else if (line == "FEATURE") {
        return FEATURE;
    } else if (line == "CONDITIONS") {
        return CONDITIONS;
    } else if (line == "MOBSPAW") {
        return MOBSPAW;
    } else if (line == "ITEMSPAW") {
        return ITEMSPAW;
    } else {
        return -1;
    }
}

void Start::printFileErr(string said, int line) {
    cout << "FILE SYNTAX (line " << line << ") " << said << endl;
}
void Start::printFileProb(string said, int line) {
    cout << "FILE READPROB (line " << line << ") " << said << endl;
}

bool Start::errorChecker(string filename) {
    int lineNum = 0;

    ifstream fin;
    fin.open(filename.c_str(), ios::in);
    if (fin.fail()) {
        cout << "FILE ERROR: Problem opening file " << filename << ".";
        return false;
    }

    bool comment = false;
    bool finished = false;
    string line = "";
    int status = BETWEEN;
    while(getline(fin, line)) {
        lineNum++;
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
            if (!finished) printFileErr("Premature end.", lineNum);
            status = BETWEEN;
            continue;
        }
        switch(status) {
            case BETWEEN:
                finished = false;
                if (line == "END") {
                    printFileErr("Duplicate END.", lineNum);
                } else {
                    status = stringToStatus(line);
                    if (status == -1) {
                        printFileErr("Not a valid status.", lineNum);
                        return true;
                    }
                } continue;
            case MTG: if (isNum(line)) printFileErr("First line of MTG must be a string (the name of the group).", lineNum); break;
            case (MTG + 1):
                if (line[1] != '-') printFileErr("Second char must be a '-'.", lineNum);
                finished = true; continue;
            case TILE: if (isNum(line)) printFileErr("First line of TILE must be a string (the name of the Tile).", lineNum); break;
            case (TILE + 1): if (!isPair(line)) printFileErr("Second line of TILE must be a valid pair (the graphic).", lineNum); break;
            case (TILE + 2):
                if (!isPair(line)) printFileErr("Third line of TILE must be a valid pair (the type and stuff).", lineNum);
                finished = true; break;
            case (TILE + 3): printFileErr("TILE only needs 3 lines.", lineNum); break;
            case WORLD: if (!isNum(line)) printFileErr("First line of WORLD must be an integer (world map width).", lineNum); break;
            case (WORLD + 1):
                if (!isNum(line)) printFileErr("Second line of WORLD must be an integer (world map height).", lineNum);
                finished = true; break;
            case (WORLD + 2): printFileErr("WORLD only needs 2 lines.", lineNum); break;
            case AREA: if (isNum(line)) printFileErr("First line of AREA must be a string (the name of the Area).", lineNum); break;
            case (AREA + 1):
                if (!isPair(line)) printFileErr("Second line of AREA must be a pair (the dimensions).", lineNum);
                finished = true; break;
            case (AREA + 2): printFileErr("AREA only needs 2 lines.", lineNum);
            case FEATURE: finished = true; status = MAP + 3; continue;
            case PLAYER: if (isNum(line)) printFileErr("First line of PLAYER must be a string (the name of the Player).", lineNum); break;
            case (PLAYER + 4):
                if (!isPair(line)) printFileErr("Fifth line of PLAYER must be a pair (the initial location).", lineNum);
                finished = true; break;
            case (PLAYER + 6):
                if (line[0] == '-') {
                } else continue;
            case (PLAYER + 5):
                switch(line[1]) {
                    case 'e': status = PLAYER + 6; continue;
                    default: printFileErr("Nope, only option is 'equips'.", lineNum); continue;
                } break;
            case RESOURCES: finished = true; continue;
            case FILES: finished = true; continue;
            case (CONNECTION + 2): if (!isPair(line)) printFileErr("Third line of CONNECTION must be a pair (location in first zone).", lineNum); break;
            case (CONNECTION + 4):
                if (!isPair(line)) printFileErr("Fifth line of CONNECTION must be a pair (location in second zone).", lineNum);
                finished = true; break;
            case (CONNECTION + 5): printFileErr("CONNECTION only needs 5 lines.", lineNum);
            case ITEM: if (isNum(line)) printFileErr("First line of ITEM must be a string (the name of the ItemType).", lineNum); break;
            case (ITEM + 1): if (isNum(line)) printFileErr("Second line of ITEM must be a string (the description of the ItemType).", lineNum); break;
            case (ITEM + 2): if (!isNum(line)) printFileErr("Third line of ITEM must be an integer (location in texture).", lineNum); break;
            case (ITEM + 3): if (!isNum(line)) printFileErr("Fourth line of ITEM must be an integer (index of texture).", lineNum); break;
            case (ITEM + 4):
                if (!isNum(line)) printFileErr("Fifth line of ITEM must be an integer (ItemType type).", lineNum);
                finished = true; break;
            case (ITEM + 5): {
                if (line[0] == '*') {
                    unsigned int i = 1;
                    while(line[i] != ',') {
                        i++;
                        if (i == line.size()) {
                            printFileErr("Where's the comma?", lineNum);
                            break;
                        }
                    }
                    if (!isNum(line.substr(1, i - 1))) printFileErr("After the asterisk must be a number (and then a comma and then a word)!", lineNum);
                } else {
                    unsigned int i = 0;
                    while(line[i] != '=') {
                        i++;
                        if (line[i] == ' ') {
                            printFileErr("No spaces please (i).", lineNum);
                            break;
                        } else if (i + 1 == line.size()) {
                            printFileErr("There has to be an equals (i).", lineNum);
                            return false;
                        }
                    }
                    if (!isNum(line.substr(i + 1, 100))) printFileErr("The right side of the equals must be an integer (i).", lineNum);
                } } continue;
            case (STATS + 1): finished = true; continue;
            case (CONDITIONS + 1): finished = true; continue;
            case UNIT: {
                if (!isSPair(line)) printFileErr("First line of UNIT must be a pair (name:code).", lineNum);
                pair<string, string> names = stps(line);
                if (names.second.size() != 6) printFileErr("Unit codes must be exactly 6 characters.", lineNum);
                } break;
            case (UNIT + 1): {
                finished = true;
                unsigned int i = 0;
                if (line[0] != '*') {
                    while(line[i] != '=') {
                        i++;
                        if (line[i] == ' ') {
                            printFileErr("No spaces please (u).", lineNum);
                            break;
                        } else if (i + 1 == line.size()) {
                            printFileErr("There has to be an equals (u).", lineNum);
                            return false;
                        }
                    }
                    if (line.substr(0, i) == "split") status = UNIT + 2;
                    else if (!isNum(line.substr(i + 1, 100))) printFileErr("The right side of the equals must be an integer (u).", lineNum);
                } } continue;
            case (UNIT + 2): status = UNIT + 1; continue;
            case MOBSPAW:
                finished = false;
                if (line[line.size() - 1] != '{') printFileErr("The first line must end with a '{'.", lineNum);
                else if (line.size() > 1 && line[line.size() - 2] == ' ') printFileErr("No spaces before the bracket.", lineNum); break;
            case (MOBSPAW + 1): if (line != "mobs[") printFileErr("The first line after the zone name is expected to be \"mobs[\"", lineNum); break;
            case (MOBSPAW + 2): {
                if (line[0] == ']') {
                    if (line == "]items[") status = MOBSPAW + 3;
                    else if (line[1] == '}') {
                        status = MOBSPAW;
                        finished = true;
                    } else printFileErr("Invalid whatever.", lineNum);
                } else {
                    if (line[0] < 48) printFileErr("The first character here is invalid.", lineNum);
                    if (line[1] != ':') printFileErr("The second character here must be ':'", lineNum);
                    for (unsigned int i = 10; i < line.size(); i += 10) {
                        if (line[i - 2] != '*' || !isNum(line.substr(i - 1, 2))) printFileErr("Incorrect formatting.", lineNum);
                    } for (unsigned int i = 11; i < line.size(); i += 10) {
                        if (line[i] != ',') printFileErr("Incorrect formattings.", lineNum);
                    }
                } continue;
            }
            case (MOBSPAW + 3): {
                if (line[0] == ']') {
                    status = MOBSPAW;
                    finished = true;
                } else {
                    unsigned int c = 0;
                    while (line[c] != ':') {
                        c++;
                        if (c == line.size()) {
                            printFileErr("There needs to be a colon.", lineNum);
                            return false;
                        }
                    }
                    if (c == 0) printFileErr("There needs to be something before the colon.", lineNum);
                    else if (c == line.size() - 1) printFileErr("There needs to be something after the colon.", lineNum);
                    else {
                        string s = line.substr(c + 1, 100);
                        if (!isPair(s)) printFileErr("There needs to be a pair after the colon.", lineNum);
                    }
                } continue;
            }
            case ITEMSPAW:
                finished = false;
                if (line[line.size() - 1] != '{') printFileErr("The first line must end with a '{'.", lineNum);
                else if (line.size() > 1 && line[line.size() - 2] == ' ') printFileErr("No spaces before the bracket.", lineNum); break;
            case (ITEMSPAW + 1): {
                if (line[0] == '}') {
                    status = ITEMSPAW;
                    finished = true;
                } else {
                    unsigned int c = 0;
                    while (line[c] != ':') {
                        c++;
                        if (c == line.size()) {
                            printFileErr("There needs to be a colon.", lineNum);
                            return false;
                        }
                    }
                    if (c == 0) printFileErr("There needs to be something before the colon.", lineNum);
                    else if (c == line.size() - 1) printFileErr("There needs to be something after the colon.", lineNum);
                    else {
                        string s2 = line.substr(c + 1, 100);
                        if (s2 == "eeverywhere" || s2 == "everywhere" || s2 == "very common" || s2 == "common" || s2 == "uncommon" || s2 == "rare") {
                        } else if (s2 == "very rare" || s2 == "extremely rare" || s2 == "rrare" || s2 == "nonexistant") {
                        } else {
                            if (!isNum(s2)) printFileErr("That is not a valid rarity.", lineNum);
                        }
                    }
                }
                } continue;
            case (MAP + 1):
                if (line == "[[") {
                    status = MAP + 10;
                } else if (line == "[") {
                    status = MAP + 2;
                } continue;
            case (MAP + 2):
                if (line[0] == ']') {
                    status = MAP + 3;
                    finished = true;
                } continue;
            case (MAP + 3): {
                char t = line[0];
                if (t == '$' || t == '@' || t == '^' || t == '<' || t == '.') {
                    status = MAP + 4;
                } else {
                    printFileErr("Not a valid map modification character.", lineNum);
                } } continue;
            case (MAP + 4):
                if (!isPair(line)) printFileErr("Expected a pair on this line (coordinates).", lineNum);
                status = MAP + 3; continue;
            case (MAP + 10): if (!isPair(line)) printFileErr("This line should be a pair.", lineNum); break;
            case (MAP + 11):
                if (line == "]]") {
                    status = MAP + 3;
                    finished = true;
                } else if (!isPair(line)) {
                    printFileErr("This line should either have a pair or double closing brackets.", lineNum);
                } continue;
            case MAPSTACK: if (isNum(line)) printFileErr("This line should be a name (of the map stack)", lineNum); break;
            case (MAPSTACK + 1): if (line.substr(0, 3) != "in ") printFileErr("The format of this line is \"in AREA\"", lineNum); break;
            case (MAPSTACK + 2): if (line.substr(0, 3) != "lt " || !isNum(line.substr(3, 100))) printFileErr("The format of this line is \"lt NUMBER\"", lineNum); break;
            case (MAPSTACK + 3): if (line.substr(0, 3) != "sp " || !isNum(line.substr(3, 100))) printFileErr("The format of this line is \"sp NUMBER\"", lineNum); break;
            case (MAPSTACK + 4): if (line.substr(0, 3) != "lv " || !isPair(line.substr(3, 100))) printFileErr("The format of this line is \"lv NUM,NUM\"", lineNum); break;
            case (MAPSTACK + 5): if (line.substr(0, 3) != "ge " || !isNum(line.substr(3, 100))) printFileErr("The format of this line is \"ge NUMBER\"", lineNum); break;
            case (MAPSTACK + 6):
                if (line[0] == ':') continue;
                else if (line[1] == '-') continue;
                else if (!isPair(line)) printFileErr("This line is not understood.", lineNum);
                break;
            case (MAPSTACK + 7):
                finished = true;
                if (!isPair(line)) printFileErr("There should be a pair of integers here.", lineNum); break;
            default: break;
        }
        status++;
    }
    return true;
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
                pair<int, int> nums = stp(line);
                tempValues[0] = nums.first;
                tempValues[1] = nums.second;
                } break;
            case (TILE + 2): {
                pair<int, int> nums = stp(line);
                tiles[tempStr] = new Tile(tempValues[0], tempValues[1], nums.first, nums.second);
                } break;
            case WORLD: tempValues[0] = sti(line); break;
            case (WORLD + 1):
                w->setDim(tempValues[0], sti(line));
                break;
            case FILES: {
                string fName = "data/" + line + ".txt";
                openFile(fName, w, p);
                continue; }
            case AREA: tempStr = line; break;
            case (AREA + 1): {
                pair<int, int> nums = stp(line);
                Area* newArea = new Area(nums.first, nums.second);
                areas[tempStr] = newArea;
                areaZones[tempStr] = new vector<Zone*>;
                w->addArea(newArea);
                }tempValues[0] = sti(line); break;
            case MAPSTACK:
                tempStr = line;
                tempMap = new map<char, string>;
                break;
            case (MAPSTACK + 1): {
                string s = line.substr(3, 100);
                if (ERRCHECK && areaZones.find(s) == areaZones.end()) printFileProb("That area simply does not exist.", lineNum);
                tempStr2 = s;
                } break;
            case (MAPSTACK + 2): tempValues[2] = sti(line.substr(3, 100)); break;
            case (MAPSTACK + 3): tempValues[3] = sti(line.substr(3, 100)); break;
            case (MAPSTACK + 4): {
                pair<int, int> nums = stp(line.substr(3, 100));
                tempValues[6] = nums.first;
                tempValues[7] = nums.second;
                } break;
            case (MAPSTACK + 5): tempValues[4] = sti(line.substr(3, 100)); break;
            case (MAPSTACK + 6):
                if (line[0] == ':') {
                    string s = line.substr(1, 100);
                    if (ERRCHECK && tileGroups.find(s) == tileGroups.end()) printFileProb("That tile group simply does not exist.", lineNum);
                    map<char, string>* temp = tileGroups[s];
                    tempMap->insert(temp->begin(),temp->end());
                    continue;
                } else if (line[1] == '-') {
                    (*tempMap)[line[0]] = line.substr(2, 100);
                     continue;
                } else {
                    pair<int, int> nums = stp(line);
                    tempValues[0] = nums.first;
                    tempValues[1]= nums.second;
                    break;
                }
            case (MAPSTACK + 7): {
                pair<int, int> nums = stp(line);
                int stairXs[nums.second];
                int stairYs[nums.second];
                int currMobSpawnLevel = tempValues[6] - 1;
                for (int i = 0; i < nums.first; i++) {
                    if (!(i % tempValues[7])) {
                        currMobSpawnLevel++;
                    }
                    Zone* newZone = new Zone(tempValues[0], tempValues[1], tempMap->size(), tempValues[2], -tempValues[4], tempValues[3]);
                    newZone->setMobSpawnLevel(currMobSpawnLevel);
                    for (map<char, string>::iterator it = tempMap->begin(); it != tempMap->end(); it++) {
                        newZone->addTile(tiles[it->second]);
                    }
                    areaZones[tempStr2]->push_back(newZone);
                    string zoneName = tempStr + its(i + 1);
                    zones[zoneName] = newZone;
                    for (int j = 0; j < nums.second; j++) {
                        if (i > 0) {
                            connection c;
                            int x = stairXs[j]; int y = stairYs[j];
                            c.x1 = x; c.y1 = y;
                            c.x2 = x; c.y2 = y;
                            c.z1 = tempZone; c.z2 = newZone;
                            areas[tempStr2]->addConnection(c);
                            newZone->addModification(x, y, '<', S_STAIRUP);
                        }
                        if (i < nums.first - 1) {
                            int x = rand() % (tempValues[0] - 4) + 2;
                            int y = rand() % (tempValues[1]- 4) + 2;
                            stairXs[j] = x; stairYs[j] = y;
                            newZone->addModification(x, y, '<', S_STAIRDOWN);
                        }
                    }
                    status = MAP + 12;
                    tempZone = newZone;
                } } break;
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
                    case '@': if (line[1] == '@') {
                        tempValues[3] = sti(line.substr(2, 100)); continue;
                    }
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
                        Zone* newZone = new Zone(wid, hei, tempMap->size(), tempValues[0], true, tempValues[3]);
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
                    case '.': //floor
                        tempStr = line.substr(1, 100);
                        status = MAP + 8;
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
                //Unit* newUnit = new Unit(tempStr, getMob(mobMap[tempStr]).second);
                Unit* newUnit = new Unit(tempStr, getMob(tempStr).second);
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
            case (MAP + 8): {
                //pair<int, int> nums = stp(line);
                if (tempZone->getNumTiles() == 0) {
                    printFileProb("Hmm, I don't like that.", lineNum);
                } else {
                    //tempZone->getLocationAt(nums.first, nums.second)->tile = //TODO this
                }
                status = MAP + 3;
                } continue;
            case (MAP + 10): {
                pair<int, int> nums = stp(line);
                tempZone = new Zone(nums.first, nums.second, tempMap->size(), tempValues[0], false, tempValues[3]);
                } break;
            case (MAP + 11):
                if (line == "]]") {
                    for (map<char, string>::iterator i = tempMap->begin(); i != tempMap->end(); i++) {
                        tempZone->addTile(tiles[i->second]);
                    }
                    areaZones[tempStr2]->push_back(tempZone);
                    zones[tempStr] = tempZone;
                    status = MAP + 12;
                } else {
                    pair<int, int> nums = stp(line);
                    tempZone->addModification(nums.first, nums.second, '0', 0);
                } continue;
            case (MAP + 12):
                tempChar = line[0];
                tempStr = line.substr(1, 100);
                break;
            case (MAP + 13): {
                pair<int, int> nums = stp(line);
                switch(tempChar) {
                    case '$': tempZone->addModification(nums.first, nums.second, tempChar, itemTypeMap[tempStr]); break;
                    case '@': /*TODO this*/ break;
                    case '^': tempZone->addModification(nums.first, nums.second, tempChar, sti(tempStr)); break;
                    case '<': tempZone->addModification(nums.first, nums.second, tempChar, getStructIntFromChar(tempStr[0])); break;
                    case '.': /*TODO this*/ break;
                    default: break;
                }
                status = MAP + 12;
                } continue;
            case FEATURE:
                if (ERRCHECK && zones.find(line) == zones.end()) {
                    printFileProb("No zone with that name has been defined yet.", lineNum);
                    break;
                } else {
                    tempZone = zones[line];
                    status = MAP + 12;
                    continue;
                }
            case PLAYER: p->setName(line); break;
            case (PLAYER + 1): {
                mob unitProto = getMob(line);
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
            case CONNECTION:
                if (ERRCHECK && areas.find(line) == areas.end()) printFileProb("No area with that name has been defined.", lineNum);
                tempArea = areas[line]; break;
            case (CONNECTION + 1):
                if (ERRCHECK && zones.find(line) == zones.end()) printFileProb("No zone with that name has been defined.", lineNum);
                tempZone = zones[line]; break;
            case (CONNECTION + 2): {
                pair<int, int> nums = stp(line);
                tempValues[0] = nums.first;
                tempValues[1]= nums.second;
                } break;
            case (CONNECTION + 3):
                if (ERRCHECK && zones.find(line) == zones.end()) printFileProb("No zone with that name has been defined.", lineNum);
                tempZone2 = zones[line]; break;
            case (CONNECTION + 4): {
                pair<int, int> nums = stp(line);
                connection c;
                c.x1 = tempValues[0];
                c.y1 = tempValues[1];
                c.x2 = nums.first;
                c.y2 = nums.second;
                c.z1 = tempZone;
                c.z2 = tempZone2;
                tempArea->addConnection(c);
                status = CONNECTION + 1;
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
                addMob(strs.first, strs.second, newUnit);
            } break;
            case (UNIT + 1): {
                StatHolder* unitPrototype = getMob(tempStr).second;
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
                getMob(tempStr).second->addStatV(S_SPAWN, hashMob(line));
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
            default: status = BETWEEN; break;
            case MOBSPAW: tempValues[0] = addEnvironment(line.substr(0, line.size() - 1)); break;
            case (MOBSPAW + 1): break;
            case (MOBSPAW + 2): {
                if (line[0] == ']') {
                    if (line[1] == 'i') status = MOBSPAW + 3;
                    else status = MOBSPAW;
                } else {
                    vector<pair<mob, unsigned int> >* encounters = new vector<pair<mob, unsigned int> >;
                    int level = cti(line[0]);
                    int mon = 0;
                    while ((mon + 1) * 10 < (signed int)line.size()) {
                        string monName = line.substr(mon * 10 + 2, 6);
                        int weight = sti(line.substr(mon * 10 + 9, 2));
                        mob theMob = getMob(monName);
                        if (ERRCHECK && theMob.first == "x") printFileProb("This mob does not exist.", lineNum);
                        encounters->push_back(pair<mob, unsigned int>(getMob(monName), weight));
                        mon++;
                    }
                    addEncounters(tempValues[0], level, encounters);
                } continue;
            }
            case (MOBSPAW + 3): {
                if (line[0] == ']') {
                    status = MOBSPAW;
                } else {
                    int c = 0;
                    while (line[c] != ':') c++;
                    string s = line.substr(0, c);
                    pair<int, int> nums = stp(line.substr(c + 1, 100));
                    for (int i = nums.first; i <= nums.second; i++) {
                        addItemsToEncounterLevel(tempValues[0], i, s);
                    }
                } continue;
            }
            case ITEMSPAW: tempValues[0] = addItemSpawnSet(line.substr(0, line.size() - 1)); break;
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
                    addItemToSpawnSet(itemTypeI, weight, minS, maxS, tempValues[0]);
                }
            } continue;
        }
        status++;
    }
}

void Start::deleteData() {
    tiles.clear();
    areas.clear();
    zones.clear();
    textures.clear();
    clearItemTypes();
    glDeleteLists(base, 96);
}
