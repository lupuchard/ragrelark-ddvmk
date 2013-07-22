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

#include "Formula.h"
#include <string>

#ifndef STAT_H
#define STAT_H

struct Skill {
    int category;
    int index;
    String name;
};

///This defines a type of statistic.
class Stat {
    public:
        /// No two stats of the same isFloat status can have the same index.
        Stat(String name, Formula* formula, unsigned char index, bool isFloat);

        /// This is mostly used for parsing, but may later be used in-game.
        String getName();

        /// The formula that defines the statistic. A formula with a length of 1 (assumed to be either SLF or some constant) will never be recalled.
        Formula* getFormula();

        bool isItFloat();
        unsigned char getIndex();

        //statics
        static void parseAll(YAML::Node fileNode);
        static Formula* parseFormula(String line, std::set<std::pair<VOwner, Stat*> >& stats, std::set<Skill*>& skills);
        static Stat* get(VOwner type, int statI);
        static Stat* get(String name);
        static bool has(String name);
        static int getNum(VOwner type);

        typedef std::pair<VOwner, Stat*> CompleteStat;
        static void setFormUser(FormulaUser* formUser);
        static FormulaUser* getFormUser();
        static std::set<Stat*> getItemAfflictions(); //Returns every stat whose formula contains an item.
        static std::set<Stat*> getEnemyAfflictions(); //Returns every stat whose formula contains an enemy.
        static std::set<Stat*> getSkillAfflictions(Skill* skill); //Returns every stat whose formula contains this skill.
        static std::vector<CompleteStat>* getAfflictions(Stat* s, VOwner ownerType);
        static std::vector<CompleteStat>* getConAfflictions(int condition);
        static void addAffliction(CompleteStat afflictingStat, Stat* afflictedStat, VOwner edOwner);
        static void addConAffliction(int afflictingCon, VOwner afflictingConOwner, Stat* afflictedStat, VOwner edOwner);
        static void addSkillAffliction(Skill* skill, Stat* afflictedStat);

        static void parseSkills(YAML::Node fileNode);
        static Skill* getSkill(int index);
        static Skill* getSkill(String name);
        static bool hasSkill(String name);
        static int getNumSkills();

        static int EXP, LEVEL, EXPREQ, STR, CON, AFF, INT, PER, DEX, CHA, LUCK, DEFENSE,
           LEARN, MAXHP, HP, MAXMANA, MANA, ACC, EVA, UNARMDAMAGE, MELDAMAGE, RANDAMAGE, RESPOIS,
           HUNGER, STAMINA, AI, BLOOD, SPLATTER, SPLIT, SWARM, UNARMED, SPAWN, LOAD,
           ATTACKSPEED, MOVESPEED, HPREGEN, MANAREGEN, METABOLISM, FLY, PET,
           POIS_PHYS, POIS_MENT, POIS_REGEN, POIS_EXTRA, CONFUSION,
           AFFLICTION, AFFLICTION_POTENCY,
           GLOC, GTEX, GTYPE;
        static int WEIGHT, VALUE, FEED, TASTE, RANGE, LIGHT, GSTACK, THRO;
    private:
        Formula* formula;
        String name;
        bool isFloat;
        unsigned char index;

        //statics
        static void parseSection(YAML::Node node, VOwner owner);
        static int add(VOwner type, Stat* theStat);
        static std::vector<Stat*> unitStats;
        static std::vector<Stat*> worldStats;
        static std::vector<Stat*> itemStats;
        static std::map<String, Stat*> statNameMap;

        static FormulaUser* formUser;
        static std::map<CompleteStat, std::vector<CompleteStat>*> afflictions;
        static std::map<int, std::vector<CompleteStat>*> conAfflictions;
        static std::set<Stat*> itemAfflictions;
        static std::set<Stat*> enemyAfflictions;
        static std::map<Skill*, std::set<Stat*> > skillAfflictions;

        static std::vector<Skill> skills;
        static std::vector<String> skillCategories;
        static std::map<String, Skill*> skillNameMap;

        static bool donnit;
        static std::map<String, int*> bindMap;
};

#endif // STAT_H
