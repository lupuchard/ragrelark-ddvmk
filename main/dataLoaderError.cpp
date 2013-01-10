#include "Start.h"
#include "dataLoader.h"

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
            case TILEDMAPS: finished = true; continue; //TODO this error check
            case TILEDMAPSREFER: finished = true; continue; //TODO this error check
            default: break;
        }
        status++;
    }
    return true;
}
