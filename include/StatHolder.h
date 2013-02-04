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

//I don's use enums for these because they have strict indices
#define S_WEIGHT 0
#define S_VALUE 1
#define S_FEED 2
#define S_TASTE 3
#define S_IDAMAGE 4
#define S_DTYPE 5
#define S_AC 6
#define S_CAPACITY 7
#define S_PENALTY 20
#define S_LIGHT 28
#define S_GSTACK 29

#define S_EXP 0
#define S_LEVEL 1
#define S_EXPREQ 2

#define S_BSTR 3
#define S_BCON 4
#define S_BAFF 5
#define S_BINT 6
#define S_BPER 7
#define S_BDEX 8
#define S_BCHA 9
#define S_BDEF 10
#define S_STR 12
#define S_CON 13
#define S_AFF 14
#define S_INT 15
#define S_PER 16
#define S_DEX 17
#define S_CHA 18
#define S_DEFENSE 20
#define S_LEARN 21
#define S_MAXHP 22
#define S_HP 23
#define S_MAXMANA 24
#define S_MANA 25
#define S_ACC 26
#define S_EVA 27
#define S_MCRITC 28
#define S_CRITC 29
#define S_HITC 30
#define S_SCRAPEC 31
#define S_UNARMDAMAGE 32
#define S_MELDAMAGE 33
#define S_RANDAMAGE 34
#define S_SPEDAMAGE 35
#define S_RESPOIS 36
#define S_RESCOLD 37
#define S_RESBURN 38
#define S_HUNGER 39
#define S_THIRST 40
#define S_STAMINA 41
#define S_AI 42
#define S_BLOOD 43
#define S_SPLATTER 44
#define S_SPLIT 45
#define S_UNARMED 47
#define S_SPAWN 48
#define S_LOAD 49
#define S_BATTACKSPEED 50
#define S_BMOVESPEED 51
#define S_ATTACKSPEED 52
#define S_MOVESPEED 53
#define S_HPREGEN 54
#define S_MANAREGEN 55
#define S_METABOLISM 56
#define S_ATTCOND 57
#define S_ATTCONDCHANCE 58
#define S_POISON 59
#define S_GLOC 60
#define S_GTEX 61
#define S_GTYPE 62

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
