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

#include "Stat.h"

Stat::Stat(String n, Formula* f, unsigned char ind, bool fl): name(n) {
    formula = f;
    isFloat = fl;
    index = ind;
}

String Stat::getName() {
    return name;
}

Formula* Stat::getFormula() {
    return formula;
}

bool Stat::isItFloat() {
    return isFloat;
}

unsigned char Stat::getIndex() {
    return index;
}

std::vector<Stat*> Stat::unitStats;
std::vector<Stat*> Stat::worldStats;
std::vector<Stat*> Stat::itemStats;
std::map<String, Stat*> Stat::statNameMap;
FormulaUser* Stat::formUser;
std::map<Stat::CompleteStat, std::vector<Stat::CompleteStat>*> Stat::afflictions;
std::map<int, std::vector<Stat::CompleteStat>*> Stat::conAfflictions;
std::set<Stat*> Stat::itemAfflictions;
std::set<Stat*> Stat::enemyAfflictions;
std::map<Skill*, std::set<Stat*> > Stat::skillAfflictions;
std::vector<Skill> Stat::skills;
std::vector<String> Stat::skillCategories;
std::map<String, Skill*> Stat::skillNameMap;
bool Stat::donnit = false;
int Stat::EXP, Stat::LEVEL, Stat::EXPREQ, Stat::STR, Stat::CON, Stat::AFF, Stat::INT, Stat::PER, Stat::DEX, Stat::CHA, Stat::LUCK, Stat::DEFENSE,
    Stat::MAXHP, Stat::HP, Stat::MAXMANA, Stat::MANA, Stat::ACC, Stat::EVA, Stat::UNARMDAMAGE, Stat::MELDAMAGE, Stat::RANDAMAGE, Stat::RESPOIS,
    Stat::HUNGER, Stat::STAMINA, Stat::AI, Stat::BLOOD, Stat::SPLATTER, Stat::SPLIT, Stat::SWARM, Stat::UNARMED, Stat::SPAWN, Stat::LOAD,
    Stat::ATTACKSPEED, Stat::MOVESPEED, Stat::HPREGEN, Stat::MANAREGEN, Stat::METABOLISM, Stat::FLY, Stat::PET,
    Stat::GLOC, Stat::GTEX, Stat::GTYPE;
int Stat::WEIGHT, Stat::VALUE, Stat::FEED, Stat::TASTE, Stat::DTYPE, Stat::RANGE, Stat::LIGHT, Stat::GSTACK, Stat::THRO;
std::map<String, int*> Stat::bindMap;

void Stat::parseAll(YAML::Node fileNode) {
    if (!donnit) {
        donnit = true;
        bindMap["EXP"] = &EXP; bindMap["LEVEL"] = &LEVEL; bindMap["EXPREQ"] = &EXPREQ;
        bindMap["STR"] = &STR; bindMap["CON"] = &CON; bindMap["AFF"] = &AFF; bindMap["INT"] = &INT; bindMap["PER"] = &PER; bindMap["DEX"] = &DEX;
        bindMap["CHA"] = &CHA; bindMap["LUCK"] = &LUCK; bindMap["DEFENSE"] = &DEFENSE; bindMap["ACC"] = &ACC; bindMap["EVA"] = &EVA;
        bindMap["MAXHP"] = &MAXHP; bindMap["HP"] = &HP; bindMap["MAXMANA"] = &MAXMANA; bindMap["MANA"] = &MANA;
        bindMap["UNARMDAMAGE"] = &UNARMDAMAGE; bindMap["MELDAMAGE"] = &MELDAMAGE; bindMap["RANDAMAGE"] = &RANDAMAGE;
        bindMap["RESPOIS"] = &RESPOIS; bindMap["AI"] = &AI; bindMap["PET"] = &PET; bindMap["UNARMED"] = &UNARMED;
        bindMap["HUNGER"] = &HUNGER; bindMap["METABOLISM"] = &METABOLISM; bindMap["STAMINA"] = &STAMINA;
        bindMap["BLOOD"] = &BLOOD; bindMap["SPLATTER"] = &SPLATTER; bindMap["FLY"] = &FLY; bindMap["LOAD"] = &LOAD;
        bindMap["SPLIT"] = &SPLIT; bindMap["SWARM"] = &SWARM; bindMap["SPAWN"] = &SPAWN;
        bindMap["ATTACKSPEED"] = &ATTACKSPEED; bindMap["MOVESPEED"] = &MOVESPEED; bindMap["HPREGEN"] = &HPREGEN; bindMap["MANAREGEN"] = &MANAREGEN;
        bindMap["GLOC"] = &GLOC; bindMap["GTEX"] = &GTEX; bindMap["GTYPE"] = &GTYPE;
        bindMap["WEIGHT"] = &WEIGHT; bindMap["VALUE"] = &VALUE; bindMap["FEED"] = &FEED; bindMap["TASTE"] = &TASTE;
        bindMap["DTYPE"] = &DTYPE; bindMap["RANGE"] = &RANGE; bindMap["LIGHT"] = &LIGHT; bindMap["GSTACK"] = &GSTACK; bindMap["THRO"] = &THRO;
        for (std::map<String, int*>::iterator iter = bindMap.begin(); iter != bindMap.end(); ++iter) {
            *iter->second = -1;
        }
    }
    if (fileNode["world"]) parseSection(fileNode["world"], V_WORLD);
    if (fileNode["item"])  parseSection(fileNode["item"], V_ITEM);
    if (fileNode["unit"])  parseSection(fileNode["unit"], V_UNIT);
    for (std::map<String, int*>::iterator iter = bindMap.begin(); iter != bindMap.end(); ++iter) {
        if (*iter->second == -1) std::cout << "Fatal Warning: Stat variable '" << iter->first << "' has not been binded.\n";
    }
}
void Stat::parseSection(YAML::Node node, VOwner owner) {
    int j = 0;
    for (YAML::const_iterator jter = node.begin(); jter != node.end(); ++jter, j++) {
        YAML::Node n = *jter;
        String statName = readYAMLStr(n, "Name", "nil", "Stat lacks a name!");
        String formula = readYAMLStr(n, "Form", "SLF");
        std::set<std::pair<VOwner, Stat*> > stats;
        std::set<Skill*> skills;
        Stat* newStat;
        if (formula[0] == 'F') {
            newStat = new Stat(statName, parseFormula(formula.substr(2), stats, skills), stats.size(), true);
        } else {
            newStat = new Stat(statName, parseFormula(formula, stats, skills), stats.size(), false);
        }
        newStat->index = j;
        String bindName = readYAMLStr(n, "Bind", "x");
        if (bindName != "x") {
            if (bindMap.find(bindName) != bindMap.end()) {
                *bindMap[bindName] = newStat->getIndex();
            } else std::cout << "'" << bindName << "' is not bindable.\n";
        }
        for (std::set<std::pair<VOwner, Stat*> >::iterator i = stats.begin(); i != stats.end(); ++i) {
            addAffliction(*i, newStat, owner);
        }
        for (std::set<Skill*>::iterator i = skills.begin(); i != skills.end(); ++i) {
            addSkillAffliction(*i, newStat);
        }
        add(owner, newStat);
    }
}
Stat* Stat::get(String name) {
    return statNameMap[name];
}
Stat* Stat::get(VOwner type, int statI) {
    switch(type) {
        case V_UNIT: return unitStats[statI]; break;
        case V_WORLD: return worldStats[statI]; break;
        case V_ITEM: return itemStats[statI]; break;
        default: return unitStats[statI]; break;
    }
}
bool Stat::has(String name) {
    return (statNameMap.find(name) != statNameMap.end());
}
int Stat::add(VOwner type, Stat* stat) {
    statNameMap[stat->name] = stat;
    switch(type) {
        case V_UNIT: unitStats.push_back(stat); return unitStats.size() - 1; break;
        case V_WORLD: worldStats.push_back(stat); return worldStats.size() - 1; break;
        case V_ITEM: itemStats.push_back(stat); return itemStats.size() - 1; break;
        default: unitStats.push_back(stat); return unitStats.size() - 1; break;
    }
}
int Stat::getNum(VOwner type) {
    switch(type) {
        case V_UNIT: return unitStats.size(); break;
        case V_WORLD: return worldStats.size(); break;
        case V_ITEM: return itemStats.size(); break;
        default: return unitStats.size(); break;
    }
}

Formula* Stat::parseFormula(String line, std::set<std::pair<VOwner, Stat*> >& stats, std::set<Skill*>& skills) {
    Formula* newFormula = new Formula(1);
    int start = 0;
    line = line + " ";
    unsigned int i = 0;
    bool tempIsFloat = false;
    for (; i < line.size(); i++) {
        if (line[i] == '.') {
            tempIsFloat = true;
        } else if (line[i] == ' ') {
            String s = line.substr(start, i - start);
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
                VType type = V_STAT;
                int aStatConSkill = 0;
                switch(s[0]) {
                    case 'u': target = V_UNIT; break;
                    case 'w': target = V_WORLD; break;
                    case 'i': target = V_ITEM; break;
                    case 'z': target = V_ZONE; break;
                    case 't': target = V_TILE; break;
                    case 'a': target = V_AREA; break;
                    case 'e': target = V_ENEMY; break;
                    case '$': {
                        target = V_UNIT;
                        type = V_SKILL;
                        String skillName = s.substr(1, 100);
                        if (!hasSkill(skillName)) {
                            std::cout << "There is an ISSUE with a skill in this formula!\n";
                            return NULL;
                        }
                        Skill* skill = getSkill(skillName);
                        aStatConSkill = skill->index;
                        skills.insert(skill);
                    } break;
                    default: std::cout << "There is something WRONG with this formula!\n"; break;
                }
                if (type != V_SKILL) {
                    switch(s[1]) {
                        case '-': {
                            type = V_STAT;
                            String statName = s.substr(2, 100);
                            if (!has(statName)) {
                                std::cout << "There is an ISSUE with a stat in this formula!\n";
                                return NULL;
                            }
                            Stat* stat = get(statName);
                            aStatConSkill = stat->getIndex();
                            stats.insert(std::pair<VOwner, Stat*>(target, stat));
                        } break;
                        default: std::cout << "There is something INCORRECT about this formula!\n"; break;
                    }
                }
                newFormula->pushVar(target, type, aStatConSkill);
            }
            start = i + 1;
        }
    }
    return newFormula;
}

void Stat::setFormUser(FormulaUser* fUser) {
    formUser = fUser;
}
FormulaUser* Stat::getFormUser() {
    return formUser;
}
std::set<Stat*> Stat::getItemAfflictions() {
    return itemAfflictions;
}
std::set<Stat*> Stat::getEnemyAfflictions() {
    return enemyAfflictions;
}
std::set<Stat*> Stat::getSkillAfflictions(Skill* skill) {
    return skillAfflictions[skill];
}
std::vector<Stat::CompleteStat>* Stat::getAfflictions(Stat* s, VOwner ownertype) {
    CompleteStat cStat = std::pair<VOwner, Stat*>(ownertype, s);
    std::map<CompleteStat, std::vector<CompleteStat>*>::iterator p = afflictions.find(cStat);
    if (p != afflictions.end()) {
        return p->second;
    }
    return NULL;
}
std::vector<Stat::CompleteStat>* Stat::getConAfflictions(int condition){
    std::map<int, std::vector<CompleteStat>*>::iterator p = conAfflictions.find(condition);
    if (p != conAfflictions.end()) {
        return p->second;
    }
    return NULL;
}
void Stat::addAffliction(CompleteStat afflictingStat, Stat* afflictedStat, VOwner edOwner) {
    if (edOwner == V_UNIT) {
        switch(afflictingStat.first) {
            case V_ITEM: itemAfflictions.insert(afflictedStat); break;
            case V_ENEMY: enemyAfflictions.insert(afflictedStat); break;
            default: {
                if (afflictions.find(afflictingStat) == afflictions.end()) {
                    afflictions[afflictingStat] = new std::vector<CompleteStat>;
                }
                afflictions[afflictingStat]->push_back(CompleteStat(edOwner, afflictedStat));
            } break;
        }
    } else {
        if (afflictions.find(afflictingStat) == afflictions.end()) {
            afflictions[afflictingStat] = new std::vector<CompleteStat>;
        }
        afflictions[afflictingStat]->push_back(CompleteStat(edOwner, afflictedStat));
    }
}
void Stat::addConAffliction(int afflictingCondition, VOwner afflictingConOwner, Stat* afflictedStat, VOwner edOwner) {
    if (edOwner == V_UNIT) {
        switch(afflictingConOwner) {
            case V_ENEMY: enemyAfflictions.insert(afflictedStat); break;
            default: {
                if (conAfflictions.find(afflictingCondition) == conAfflictions.end()) {
                    conAfflictions[afflictingCondition] = new std::vector<CompleteStat>;
                }
                conAfflictions[afflictingCondition]->push_back(CompleteStat(edOwner, afflictedStat));
            }
        }
    } else {
        if (conAfflictions.find(afflictingCondition) == conAfflictions.end()) {
            conAfflictions[afflictingCondition] = new std::vector<CompleteStat>;
        }
        conAfflictions[afflictingCondition]->push_back(CompleteStat(edOwner, afflictedStat));
    }
}
void Stat::addSkillAffliction(Skill* afflictingSkill, Stat* afflictedStat) {
    std::map<Skill*, std::set<Stat*> >::iterator i = skillAfflictions.find(afflictingSkill);
    if (i == skillAfflictions.end()) {
        skillAfflictions[afflictingSkill] = std::set<Stat*>();
        skillAfflictions[afflictingSkill].insert(afflictedStat);
    } else {
        i->second.insert(afflictedStat);
    }
}

void Stat::parseSkills(YAML::Node fileNode) {
    int i = 0;
    for (YAML::Node::iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter, i++) {
        String categoryName = iter->first.as<String>();
        skillCategories.push_back(categoryName);
        for (YAML::Node::iterator jter = iter->second.begin(); jter != iter->second.end(); ++jter) {
            Skill newSkill;
            newSkill.name = jter->as<String>();
            newSkill.category = i;
            newSkill.index = skills.size();
            skills.push_back(newSkill);
            skillNameMap[newSkill.name] = &skills[skills.size() - 1];
        }
    }
}
int Stat::getNumSkills() {
    return skills.size();
}
Skill* Stat::getSkill(int index) {
    return &skills[index];
}
Skill* Stat::getSkill(String name) {
    return skillNameMap[name];
}
bool Stat::hasSkill(String name) {
    return (skillNameMap.find(name) != skillNameMap.end());
}
