#include "Start.h"

#define F_ITEM 1
#define F_UNIT 2
#define F_TILE 3
#define F_CHUNK 4
#define F_ZONE 5
#define F_AREA 6
#define F_ENEMY 7

int Start::getVarValue(VOwner target, VType type, int index, StatHolderIntef* statHolder) {
    StatHolder* foundStatHolder = findStatHolder(target, (StatHolder*)statHolder);
    switch(type) {
        case V_STAT: return foundStatHolder->getStatValue(index); break;
        case V_SKILL: break;
        case V_CONDITION: return foundStatHolder->getCondition(index);
        break;
        default: break;
    }
    return 0;
}

float Start::getVarValueF(VOwner target, VType type, int index, StatHolderIntef* statHolder) {
    StatHolder* foundStatHolder = findStatHolder(target, (StatHolder*)statHolder);
    switch(type) {
        case V_STAT: return foundStatHolder->getStatValue(index); break; //NOTE: UNABLE TO RETRIEVE FLOAT VARIABLES FROM FORMULA AT THIS TIME
        case V_SKILL: break;
        case V_CONDITION: return foundStatHolder->getCondition(index);
        break;
        default: break;
    }
    return 0;
}

void Start::statChanged(int stat, StatHolderIntef* statHolder) {
    Stat* theStat = getStat(statHolder->getOwner(), stat);
    vector<completeStat>* affStats = getAfflictions(theStat, statHolder->getOwner());
    if (affStats) {
        for (unsigned int i = 0; i < affStats->size(); i++) {
            pair<int, Stat*> cStat = (*affStats)[i];
            StatHolder* foundStatHolder = findStatHolder(cStat.first, (StatHolder*)statHolder);
            //foundStatHolder->needToUpdate(stat, theStat->isItFloat());
            foundStatHolder->needToUpdate(cStat.second->getIndex(), cStat.second->isItFloat());
            statChanged(cStat.second->getIndex(), statHolder); //not really but sorta
        }
    }
}

void Start::conditionChanged(int condition, StatHolderIntef* statHolder) {
    vector<completeStat>* affStats = getConAfflictions(condition);
    if (affStats) {
        for (unsigned int i = 0; i < affStats->size(); i++) {
            pair<int, Stat*> cStat = (*affStats)[i];
            StatHolder* foundStatHolder = findStatHolder(cStat.first, (StatHolder*)statHolder);
            foundStatHolder->needToUpdate(cStat.second->getIndex(), cStat.second->isItFloat());
            statChanged(cStat.second->getIndex(), statHolder); //not really but sorta
        }
    }
}

StatHolder emptyStatHolder = StatHolder(V_ITEM);

StatHolder* Start::findStatHolder(int target, StatHolder* statHolderFrom) {
    VOwner owner = statHolderFrom->getOwner(); //step 8
    switch(target) {
        case V_WORLD: return world; break;
        case V_ITEM: {
            switch(owner) {
                case V_UNIT: {
                    if (statHolderFrom == player->getUnit()) {
                        return primeFolder->getEquips();
                    } else {
                        //cout << "*** WARNING: other unit equipment not work yet???? ***" << endl; //TODO this
                        return &emptyStatHolder;
                    }
                } break;
                case V_ITEM: return statHolderFrom; break;
                case V_TILE: cout << "*** WARNING: TILE ITEM PILE STAT OBTAINING NOT IMPLEMENTED YET ***" << endl; break;
                default: cout << "*** WARDING: you are doing it wrong " << target << " " << owner << endl; break;
            }
            return 0;
        } break;
        case V_UNIT: { //step 9
            switch(owner) {
                case V_UNIT: return statHolderFrom; break; //step 10
                case V_ITEM: cout << "*** WARNING: hell this wouldn't even work what am i even doing ***" << endl; break;
                case V_WORLD: return player->getUnit(); break;
                case V_TILE: cout << "*** WARNING: TILE UNIT STATDING STAT OBTAINING NOT IMPLEMENTED YET ***" << endl; break;
                default: cout << "*** WARDING: you are doing it wrong " << target << " " << owner << endl; break;
            }
        } break;
        case V_TILE: {
            switch(owner) {
                case V_UNIT: cout << "*** WARNING: this would almost work easily but it doesn't so whatever ***" << endl; break;
                case V_ITEM: cout << "*** WARNING: ok this is impossible I hate dependencies ***" << endl; break;
                case V_TILE: return statHolderFrom; break;
                default: cout << "*** WARDING: you are doing it wrong " << target << " " << owner << endl; break;
            }
        } break;
        case V_ZONE: {
            switch(owner) {
                case V_UNIT: cout << "*** WARNING: i definately need to find a better way to structure this all ***" << endl; break;
                case V_ITEM: cout << "*** WARNING: what i said above ***" << endl; break;
                case V_TILE: cout << "*** WARNING: ill get better at planning later maybe ***" << endl; break;
                case V_ZONE: return statHolderFrom; break;
                case V_AREA: cout << "*** WARNING: i havent done this yet **" << endl; break;
                default: cout << "*** WARDING: you are doing it wrong " << target << " " << owner << endl; break;
            }
        } break;
        case V_AREA:
            if (owner == V_AREA) {
                return statHolderFrom;
            }
            return activeArea;
            break;
        case V_ENEMY:
            if (owner == V_UNIT) {
                return ((Unit*)statHolderFrom)->getEnemy();
            } break;
        default: break;
    }
    return 0;
}

int Start::getTime() {
    return world->theTime;
}

void Start::parseFormula(string line, bool errCheck, int lineNum) {
    Formula* newFormula = new Formula(1);
    vector<pair<int, Stat*> > statsForFormula;
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
                    default: if (errCheck) {printFileErr("There is something WRONG with this formula!", lineNum);} break;
                }
                VType type = V_STAT;
                int aStatConSkill = 0;
                switch(s[1]) {
                    case '-': {
                        type = V_STAT;
                        map<string, int>::iterator it = statMap.find(s.substr(2, 100));
                        if (it == statMap.end()) printFileErr("There is an ISSUE with a stat in this formula!", lineNum);
                        aStatConSkill = it->second;
                        statsForFormula.push_back(pair<int, Stat*>(target, getStat(target, aStatConSkill)));
                    } break;
                    case '|': {
                        type = V_CONDITION;
                        map<string, int>::iterator it = conditionMap.find(s.substr(2, 100));
                        if (it == conditionMap.end()) printFileErr("There is an ISSUE with a condition in this formula!", lineNum);
                        aStatConSkill = it->second;
                        conditionsForFormula.push_back(pair<int, int>(target, aStatConSkill));
                    } break;
                    case '~': type = V_SKILL; break;
                    default: if (errCheck) {printFileErr("There is something INCORRECT about this formula!", lineNum);} break;
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
}
