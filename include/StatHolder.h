#ifndef STATHOLDER_H
#define STATHOLDER_H

#include <iostream>
#include <bitset>
#include <unordered_map>
#include <set>
#include "Stat.h"
using namespace std;

#define MAX_HUNGER 5000
#define MAX_STAMINA 10000

enum StatsOhYes{S_EXP = 0, S_LEVEL, S_EXPREQ, S_BSTR, S_BCON, S_BAFF, S_BINT, S_BPER, S_BDEX, S_BCHA, S_BDEF, S_BLUCK, S_STR, S_CON, S_AFF, S_INT, S_PER, S_DEX, S_CHA, S_LUCK, S_DEFENSE,
                S_LEARN, S_MAXHP, S_HP, S_MAXMANA, S_MANA, S_ACC, S_EVA, S_MCRITC, S_CRITC, S_HITC, S_SCRAPEC, S_UNARMDAMAGE, S_MELDAMAGE, S_RANDAMAGE, S_SPEDAMAGE,
                S_RESPOIS, S_RESCOLD, S_RESBURN, S_HUNGER, S_THIRST, S_STAMINA, S_AI, S_BLOOD, S_SPLATTER, S_SPLIT, S_SWARM, S_UNARMED, S_SPAWN, S_LOAD,
                S_BATTACKSPEED, S_BMOVESPEED, S_ATTACKSPEED, S_MOVESPEED, S_HPREGEN, S_MANAREGEN, S_METABOLISM, S_ATTCOND, S_ATTCONDCHANCE, S_POISON, S_FLY, S_GLOC, S_GTEX, S_GTYPE};

enum StatsOhNo{S_WEIGHT, S_VALUE, S_FEED, S_TASTE, S_IDAMAGE, S_DTYPE, S_AC, S_CAPACITY, S_PENALTY = 20, S_LIGHT = 28, S_GSTACK = 29};

#define C_POISON 8
#define C_CONFUSION 34

typedef pair<VOwner, Stat*> completeStat;

void setFormUser(FormulaUser* formUser);

/* Returns every stat whose formula contains an item. */
set<Stat*> getItemAfflictions();
/* Returns every stat whose formula contains an enemy. */
set<Stat*> getEnemyAfflictions();

set<Stat*> getSkillAfflictions(SkillType skill);

vector<completeStat>* getAfflictions(Stat* s, VOwner ownerType);
vector<completeStat>* getConAfflictions(int condition);
void addAffliction(completeStat afflictingStat, Stat* afflictedStat, VOwner edOwner);
void addConAffliction(int afflictingCon, VOwner afflictingConOwner, Stat* afflictedStat, VOwner edOwner);
void addSkillAffliction(SkillType afflictionSkill, Stat* afflictedStat);

/* A StatHolder has and a manages a bunch of stats and values for them. */
/* This class was constructed with only memory in mind, so it's not necessarily as fast as it could be. */
class StatHolder: StatHolderIntef {
    public:
        StatHolder(VOwner owner);
        virtual ~StatHolder();

        /* Adds a stat to the StatHolder (with or without a value). addStatF and addStatVF add float stats. */
        void addStat(int statI);
        void addStatV(int statI, int value);
        void addStatF(int statI);
        void addStatVF(int statI, float value);

        /* Sets or modifies the value of a specific stat. */
        virtual void setStat(int stat, int value);
        virtual short modifyStat(int stat, int amount);
        virtual void setStatF(int stat, float value);
        virtual float modifyStatF(int stat, float amount);

        /* Returns the value stored for a specific stat. If a stat is not stored, it returns 0. */
        virtual short getStatValue(int stat);
        virtual float getStatValueF(int stat);

        /* Informs the StatHolder that a stat has changed. Should not need to be called outside this class because of the stat setting methods call this. */
        void statChanged(int stat);
        /* Informs the StatHolder that a stat should recalculate itself (probably because a variable in the formula has changed. */

        virtual void needToUpdate(int stat, bool isFloat);

        VOwner getOwner();

        /* Returns true if this StatHolder stores a value for this stat. */
        virtual bool hasStat(int stat, bool isFloat);

        int getNumStats();
        int getNumIntStats();
        int getNumFloatStats();

        /* Arrays containing the int and float stats respectivly. */
        unsigned char* getIntStats();
        unsigned char* getFloatStats();

        bool getCondition(int condition);
        void setCondition(int condition, bool value);
        bool hasAnyConditions();

        void makeHashMaps();
        void removeHashMaps();
    protected:
        StatHolder* aThis;
    private:
        /* Stats and their values are stored in sorted arrays (sorted by stat index) */
        unsigned char* intStats;
        short* intValues;
        bitset<32> toBeUpdatedInt;

        unsigned char* floatStats;
        float* floatValues;
        bitset<32> toBeUpdatedFloat;

        bitset<64> conditions;

        unsigned char numIntStats;
        unsigned char numFloatStats;
        unsigned char owner;
        bool hashMapped;

        int binarySearchInt(int first, int last, int stat);
        int binarySearchFloat(int first, int last, int stat);

        unordered_map<unsigned char, pair<short, bool> >* tempHashMap;
        unordered_map<unsigned char, pair<float, bool> >* tempFloatHashMap;
};

#endif // STATHOLDER_H
