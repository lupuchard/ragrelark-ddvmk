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
#define RESOURCES_LOC "resources/"

#define DEBUG false

int getStructIntFromChar(int c) {
    if (c < 50) return (c - 38) / 2 + 26;
    else        return c - 97;
}

void Start::buildFont() {
    Display *dpy;
    XFontStruct *fontInfo;
    base = glGenLists(96);
    dpy = XOpenDisplay(NULL);
    fontInfo = XLoadQueryFont(dpy, "-adobe-helvetica-medium-r-normal--18-*-*-*-p-*-iso8859-1" );
    if (fontInfo == NULL) {
        fontInfo = XLoadQueryFont(dpy, "fixed");
        if (fontInfo == NULL) {
            cout << "no X font avaliable?" << endl;
        }
    }
    glXUseXFont(fontInfo->fid, 32, 96, base);
    XFreeFont(dpy, fontInfo);
    XCloseDisplay(dpy);
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
    }

    for (map<string, vector<Zone*>*>::iterator i = areaZones.begin(); i != areaZones.end(); i++) {
        areas[i->first]->setZones(&(*(i->second))[0], i->second->size());
    }

    spawnPrototypes.clear();
    //mobMap.clear();

    buildFont();

    cout << "All done with data loading." << endl;
}

void Start::finishDataSetup() {

    for (unsigned int i = 0; i < itemsToEquip.size(); i++) {
        equipItem(itemsToEquip[i]);
    }
    //retrieveAfflictions();

    tileGroups.clear();
    areaZones.clear();
    itemTypeMap.clear();
    itemsToEquip.clear();
    statMap.clear();
    conditionMap.clear();
}

Texture* Start::getTexture(int i) {
    return textures[i];
}

void Start::loadImage(string filename) {
    cout << " -Loading " << filename << "..." << endl;

    Texture* newTexture = new Texture();
    newTexture->OnLoad("resources/" + filename);
    if (filename == "structures.png") {
        structureTex = newTexture;
        gotsStructureTex = true;
    } else if (filename == "menu.png") {
        menuTex = newTexture;
        gotsMenuTex = true;
    } else if (filename == "font.png") {
        fontTex = newTexture;
        gotsFontTex = true;
    } else if (filename == "splatters.png") {
        splatterTex = newTexture;
        gotsSplatterTex = true;
    } else if (filename == "attackAnims.png") {
        attackAnimsTex = newTexture;
        gotsAttackAnimsTex = true;
    }
    textures.push_back(newTexture);
}

void Start::openFile(string fileName, World* w, Player* p) {

    ifstream fin;
    fin.open(fileName.c_str(), ios::in);

    cout << " -Opening " << fileName << "..." << endl;

    string line = "";
    int status = BETWEEN; // The status determine what the loader is looking for next. BETWEEN means it has no particular goal at the moment.
    while(getline(fin, line)) {
        //cout << "  --Interpreting \"" << line << "\"" << endl;
        //if this line is empty of a comment, it skips to the next line

        boost::algorithm::trim(line);
        if (line.length() == 0) continue;
        if (line[0] == '~') continue;
        if (line == "END") {
            status = BETWEEN; //if this line is the end, it skips to the next line and reverts to the main status
            //cout << "    ---ENDING" << endl;
            continue;
        }
        if (DEBUG) {
            cout << "    -- " << status << ". " << line << endl;
        }
        //continue is used when I don't want the status to increment at the end
        switch(status) {
            case BETWEEN:
                if (line == "TILE") {
                    status = TILE;
                } else if (line == "MTG") {
                    status = MTG;
                } else if (line == "WORLD") {
                    status = WORLD;
                } else if (line == "PLAYER") {
                    status = PLAYER;
                } else if (line == "FILES") {
                    status = FILES;
                } else if (line == "MAP") {
                    status = MAP;
                } else if (line == "AREA") {
                    status = AREA;
                } else if (line == "MONSTER") {
                    status = MONSTER;
                } else if (line == "RESOURCES") {
                    status = RESOURCES;
                } else if (line == "CONNECTION") {
                    status = CONNECTION;
                } else if (line == "MAPSPACE") {
                    status = MAPSPACE;
                } else if (line == "ITEM") {
                    status = ITEM;
                } else if (line == "STATS") {
                    status = STATS;
                } else if (line  == "UNIT") {
                    status = UNIT;
                } else if (line == "MAPSTACK") {
                    status = MAPSTACK;
                } else if (line == "FEATURE") {
                    status = FEATURE;
                } else if (line == "CONDITIONS") {
                    status = CONDITIONS;
                } else if (line == "MOBSPAW") {
                    status = MOBSPAW;
                } else if (line == "ITEMSPAW") {
                    status = ITEMSPAW;
                }
                cout << "  --Now doing \"" << line << "\"." << endl;
                continue;
            case MTG:
                tempMap = new map<char, string>;
                tileGroups[line] = tempMap;
                break;
            case (MTG + 1):
                cout << "    ---Storing \"" << line.substr(2, 100) << "\" under '" << line[0] << "'." << endl;
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
            case (AREA + 1): tempValues[0] = sti(line); break;
            case (AREA + 2): {
                Area* newArea = new Area(tempValues[0], sti(line));
                areas[tempStr] = newArea;
                areaZones[tempStr] = new vector<Zone*>;
                w->addArea(newArea);
                } break;
            case MAPSTACK:
                tempStr = line;
                tempMap = new map<char, string>;
                break;
            case (MAPSTACK + 1): tempStr2 = line.substr(3, 100); break;
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
                    map<char, string>* temp = tileGroups[line.substr(1, 100)];
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
                    case '>': tempStr2 = line.substr(1, 100); continue;
                    case 'v': tempValues[2] = sti(line.substr(1, 100)); continue;
                    case ':': {
                        map<char, string>* temp = tileGroups[line.substr(1, 100)];
                        tempMap->insert(temp->begin(),temp->end());
                        continue; }
                    case '[':
                        if (line[1] == '[') {
                            status = MAP + 10;
                            continue;
                        } else {
                            tempVect.clear();
                        } break;
                    case '*': tempValues[0] = sti(line.substr(1, 100)); continue;
                    case '@': if (line[1] == '@') {
                        tempValues[3] = sti(line.substr(2, 100)); continue;
                    }
                    default:
                        if (line[1] == '-') {
                            (*tempMap)[line[0]] = line.substr(2, 100);
                        } continue;
                } break;
            case (MAP + 2):
                if (line[0] == ']') {
                    status = MAP + 3;
                    int wid = tempVect[0].size() / 4 + 1;
                    int hei = tempVect.size();
                    Zone* newZone = new Zone(wid, hei, tempMap->size(), tempValues[0], true, tempValues[3]);
                    int* heights = new int[wid * hei];
                    int* ntiles = new int[wid * hei];
                    int* structs = new int[wid * hei];
                    for (int i = 0; i < wid; i++) {
                        for (int j = 0; j < hei; j++) {
                            int k = i + j * wid;
                            ntiles[k] = distance(tempMap->begin(), tempMap->find(tempVect.at(j).at(i * 4)));
                            int b = tempVect.at(j).at(i * 4 + 1);
                            if (b < 60)       heights[k] = b - 48;
                            else if (b < 90) heights[k] = b - 55;
                            else              heights[k] = b - 87;
                            structs[k] = getStructIntFromChar(tempVect.at(j).at(i * 4 + 2));
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
                    cout << "ERRORERORERERRERREROR.RORERREEONEOSE WHAT DO WE DO DOD DO DO NONONONONO" << endl;
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
                tempZone = zones[line];
                status = MAP + 12;
                continue;
            case PLAYER: p->setName(line); break;
            case (PLAYER + 1): /*p->setUnitProto(getMob(mobMap[line]).second);*/ p->setUnitProto(getMob(line).second); break;
            case (PLAYER + 2): p->setArea(areas[line]); break;
            case (PLAYER + 3): p->setZone(zones[line]); break;
            case (PLAYER + 4): p->getUnit()->x = sti(line); break;
            case (PLAYER + 5):
                p->getUnit()->y = sti(line);
                p->getZone()->getLocationAt(p->getUnit()->x, p->getUnit()->y)->unit = p->getUnit();
                break;
            case (PLAYER + 7):
                if (line[0] == '-') {

                } else {
                    itemsToEquip.push_back(Item(itemTypeMap[line]));
                    continue;
                }
            case (PLAYER + 6):
                switch(line[1]) {
                    case 'e': status = PLAYER + 7; continue;
                    default: continue;
                } break;
            case RESOURCES: loadImage(line); continue;
            case MAPSPACE: areaZones[line]->push_back(NULL); break;
            case CONNECTION: tempArea = areas[line]; break;
            case (CONNECTION + 1): tempZone = zones[line]; break;
            case (CONNECTION + 2): {
                pair<int, int> nums = stp(line);
                tempValues[0] = nums.first;
                tempValues[1]= nums.second;
                } break;
            case (CONNECTION + 3): tempZone2 = zones[line]; break;
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
                int i = 0;
                while(line[i] != '=') {
                    i++;
                }
                string s = line.substr(0, i);
                int stat = statMap[s];
                ItemType* itemType = getItemType(itemTypeMap[tempStr]);
                int num = sti(line.substr(i + 1, 100));
                itemType->addStatV(stat, num);
                } continue;
            case (STATS + 2):
                if (line[0] != '-') {
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
                    }
                } else if (line[0] == '$') {
                    status = STATS + 2;
                } else {
                    tempStr = line;
                    status = STATS + 1;
                } continue;
            case (STATS + 1): {
                Formula* newFormula = new Formula(1);
                vector<pair<int, Stat*> > statsForFormula;
                ///aoeu
                vector<pair<int, int> > conditionsForFormula;
                bool isFloat = false;
                int start = 0;
                line = line + " ";
                unsigned int i = 0;
                if (line[0] == '(') {
                    if (line[1] == 'f') {
                        isFloat = true;
                    } else if (line[1] == 'i') {
                        isFloat = false;
                    }
                    i = 4;
                    start = 4;
                }
                bool tempIsFloat = false;
                for (; i < line.size(); i++) {
                    if (line[i] == '.') {
                        //isFloat = true;
                        tempIsFloat = true;
                    } else if (line[i] == ' ') {
                        string s = line.substr(start, i - start);
                        if ((s[0] >= 48 && s[0] < 58) || (s[0] == '-' && s.size() > 1) || (s[0] == '.' && s.size() > 1)) {
                            if (tempIsFloat) {
                                newFormula->pushFloat(stf(s));
                                tempIsFloat = false;
                            } else {
                                newFormula->pushInt(sti(s));
                            }
                        } else if (s == "+" || s == "ADD")  {newFormula->pushOperator(O_ADD); }
                        else if (s == "-" || s == "SUB")    {newFormula->pushOperator(O_SUB); }
                        else if (s == "*" || s == "MUL")    {newFormula->pushOperator(O_MUL); }
                        else if (s == "/" || s == "DIV")    {newFormula->pushOperator(O_DIV); }
                        else if (s == "%" || s == "MOD")    {newFormula->pushOperator(O_MOD); }
                        else if (s == "^" || s == "POW")    {newFormula->pushOperator(O_POW); }
                        else if (s == "SWP" || s == "SWAP") {newFormula->pushOperator(O_SWP); }
                        else if (s == "MAX")                {newFormula->pushOperator(O_MAX); }
                        else if (s == "MIN")                {newFormula->pushOperator(O_MIN); }
                        else if (s == "!" || s == "NOT")    {newFormula->pushOperator(O_NOT); }
                        else if (s == "=" || s == "IFE")    {newFormula->pushOperator(O_IFE); }
                        else if (s == ">" || s == "IFG")    {newFormula->pushOperator(O_IFG); }
                        else if (s == "TRU" || s == "TRUE") {newFormula->pushOperator(O_TRU); }
                        else if (s == "SLF" || s == "SELF") {newFormula->pushOperator(O_SLF); }
                        else if (s == "TIM" || s == "TIME") {newFormula->pushOperator(O_TIM); }
                        else if (s == "E" || s == "EEE")    {newFormula->pushOperator(O_EEE); }
                        else if (s == "PI" || s == "PIE")   {newFormula->pushOperator(O_PIE); }
                        else {
                            VOwner target = V_WORLD;
                            switch(s[0]) {
                                case 'u': target = V_UNIT; break;
                                case 'w': target = V_WORLD; break;
                                case 'i': target = V_ITEM; break;
                                case 'z': target = V_ZONE; break;
                                case 't': target = V_TILE; break;
                                case 'a': target = V_AREA; break;
                                case 'e': target = V_ENEMY; break;
                                default: target = V_WORLD; break;
                            }
                            VType type = V_STAT;
                            int aStatConSkill = 0;
                            switch(s[1]) {
                                case '-': {
                                    type = V_STAT;
                                    aStatConSkill = statMap[s.substr(2, 100)];
                                    statsForFormula.push_back(pair<int, Stat*>(target, getStat(target, aStatConSkill)));
                                } break;
                                case '|': {
                                    type = V_CONDITION;
                                    aStatConSkill = conditionMap[s.substr(2, 100)];
                                    conditionsForFormula.push_back(pair<int, int>(target, aStatConSkill));
                                } break;
                                case '~': type = V_SKILL; break;
                                default: type = V_STAT;
                            }
                            newFormula->pushVar(target, type, aStatConSkill);
                        }
                        start = i + 1;
                    }
                }
                Stat* newStat = new Stat(tempStr, newFormula, getNumStats((VOwner)tempValues[0]), isFloat);
                for (unsigned int i = 0; i < statsForFormula.size(); i++) {
                    addAffliction(statsForFormula[i], newStat, tempValues[0]);
                }
                for (unsigned int i = 0; i < conditionsForFormula.size(); i++) {
                    addConAffliction(conditionsForFormula[i].second, conditionsForFormula[i].first, newStat, tempValues[0]);
                }
                statMap[tempStr] = addStat((VOwner)tempValues[0], newStat);
                statsForFormula.clear();
                conditionsForFormula.clear();

                status = STATS;
            } continue;
            case UNIT: {
                pair<string, string> strs = stps(line);
                tempStr = strs.second;
                StatHolder* newUnit = new StatHolder(V_UNIT);
                addMob(strs.first, strs.second, newUnit);
                //mobMap[strs.second] = index;
            } break;
            case (UNIT + 1): {
                //int mobI = mobMap[tempStr];
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
                //getMob(mobMap[tempStr]).second->addStatV(S_SPAWN, findMob(line));
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
                    status = MOBSPAW + 3;
                } else {
                    vector<pair<mob, unsigned int> >* encounters = new vector<pair<mob, unsigned int> >;
                    int level = cti(line[0]);
                    int mon = 0;
                    while ((mon + 1) * 10 < (signed int)line.size()) {
                        string monName = line.substr(mon * 10 + 2, 6);
                        int weight = sti(line.substr(mon * 10 + 9, 2));
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
                    int c = 0;
                    while (line[c] != ':') c++;
                    string s = line.substr(0, c);
                    string s2 = line.substr(c + 1, 100);
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
                    int itemTypeI = itemTypeMap[s];
                    addItemToSpawnSet(itemTypeI, weight, tempValues[0]);
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
