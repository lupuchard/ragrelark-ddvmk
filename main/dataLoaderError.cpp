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
        cout << "FILE ERROR: Problem opening file " << filename << "." << endl;
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
            case (MTG + 1): finished = true; continue;
            case TILE: if (isNum(line)) printFileErr("First line of TILE must be a string (the name of the Tile).", lineNum); break;
            case (TILE + 1): {
                unsigned int i = 0;
                while(line[i] != ':') {
                    i++;
                    if (i == line.size()) {
                        printFileErr("Where's the colon?", lineNum);
                        break;
                    }
                }
                if (!isPair(line.substr(0, i))) printFileErr("Second line of TILE must have a valid pair (the graphic coords).", lineNum);
                if (!isNum(line.substr(i + 1, 100))) printFileErr("Second line of TILE must have a texture index after the colon.", lineNum);
                } break;
            case (TILE + 2):
                if (!isPair(line)) printFileErr("Third line of TILE must be a valid pair (the type and stuff).", lineNum);
                finished = true; break;
            case (TILE + 3):
                if (line[0] == '*') {
                    if (line.size() == 3) {
                        if (!(line[1] == 't' || line[1] == 'f') || !(line[2] == 't' || line[2] == 'f')) printFileErr("t's and f's only please", lineNum);
                    } else printFileErr("Only two chars, t or f each, after the asterix.", lineNum);
                } else if (isNum(line)) printFileErr("No number on the fourth line of TILE, silly!", lineNum);
                break;
            case (TILE + 4): printFileErr("TILE only needs 3 or 4 lines.", lineNum); break;
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
            case (STAIRS + 2): if (!isPair(line)) printFileErr("Third line of STAIRS must be a pair (location in first zone).", lineNum); break;
            case (STAIRS + 3): finished = true; break;
            case (STAIRS + 4): if (!isPair(line)) printFileErr("Fifth line of STAIRS must be a pair (location in second zone).", lineNum); break;
            case (STAIRS + 5): printFileErr("STAIRS only needs 5 lines.", lineNum);
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
                } else if (line[0] == '(') {
                    if (line[line.size() - 1] != ')') printFileErr("Close yer parthens man.", lineNum);
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
                        if (line[i - 9] == '(') {
                            if (line[i] != ')' || line[i - 5] != ',') printFileErr("Incorrect farmatting.", lineNum);
                        } else {
                            if (line[i - 2] != '*' || !isNum(line.substr(i - 1, 2))) printFileErr("Incorrect formatting.", lineNum);
                        }
                    } for (unsigned int i = 11; i < line.size(); i += 10) {
                        if (line[i] != ',' && line[i] != '(') printFileErr("Incorrect formattings.", lineNum);
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
            case MAPSTACK: if (isNum(line)) printFileErr("This line should be a name (of the map stack)", lineNum); break;
            case (MAPSTACK + 1): if (line[0] != '>') printFileErr("The format of this line is \">AREA\"", lineNum); break;
            case (MAPSTACK + 2): {
                finished = true;
                unsigned int b = 0;
                while (line[b] != ' ' && b < line.size()) b++;
                b++;
                if (b > 3) {
                    while (b < line.size()) {
                        if (line[b] != '+' && line[b] != '.' && line[b] != ',' && !isNum(line.substr(b, 1))) printFileErr("inorrwrong", lineNum);
                        b++;
                    }
                } else printFileErr("wrong", lineNum);
                } continue;
            if (line.substr(0, 3) != "lt " || !isNum(line.substr(3, 100))) printFileErr("The format of this line is \"lt NUMBER\"", lineNum); break;
            case TILEDMAPS: finished = true; continue; //TODO this error check
            case TILEDMAPSREFER: finished = true; continue; //TODO this error check
            case SKILLS: finished = true; continue; //TODO this error check
            case MOBEQUIPS: finished = true; continue; //TODO this error check
            case SPELLS: finished = true; continue; //TODO this error check
            default: break;
        }
        status++;
    }
    return true;
}
