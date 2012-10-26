#ifndef STATHOLDER_H
#define STATHOLDER_H

#include <iostream>
#include <bitset>
#include "Stat.h"
using namespace std;

#define MAX_HUNGER 10000
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
#define S_LIGHT 25

#define S_EXP 0
#define S_LEVEL 1
#define S_EXPREQ 2
#define S_LOC 3
#define S_TEX 4
#define S_BSTR 5
#define S_BCON 6
#define S_BAFF 7
#define S_BINT 8
#define S_BPER 9
#define S_BDEX 10
#define S_BCHA 11
#define S_BDEF 12
#define S_STR 14
#define S_CON 15
#define S_AFF 16
#define S_INT 17
#define S_PER 18
#define S_DEX 19
#define S_CHA 20
#define S_DEFENSE 21
#define S_MAXHP 23
#define S_HP 24
#define S_MAXMANA 25
#define S_MANA 26
#define S_ACC 27
#define S_EVA 28
#define S_MCRITC 29
#define S_CRITC 30
#define S_HITC 31
#define S_SCRAPEC 32
#define S_DAMAGE 33
#define S_HUNGER 34
#define S_THIRST 35
#define S_STAMINA 36
#define S_AI 37
#define S_BLOOD 38
#define S_SPLATTER 39
#define S_SPLIT 40
#define S_UNARMED 42
#define S_SPAWN 43
#define S_LOAD 44
#define S_ATTACKSPEED 47
#define S_MOVESPEED 48
#define S_HPREGEN 49
#define S_MANAREGEN 50
#define S_METABOLISM 51
#define S_ATTCOND 52
#define S_ATTCONDCHANCE 53
#define S_POISON 54

#define C_POISON 8
#define C_CONFUSION 34

typedef pair<int, Stat*> completeStat;
//typedef pair<int, int> completeCon;

void setFormUser(FormulaUser* formUser);

/* Returns every stat whose formula contains an item. */
vector<Stat*> getItemAfflictions();
/* Returns every stat whose formula contains an enemy. */
vector<Stat*> getEnemyAfflictions();

vector<completeStat>* getAfflictions(Stat* s, int ownerType);
vector<completeStat>* getConAfflictions(int condition);
void addAffliction(completeStat afflictingStat, Stat* afflictedStat, int edOwner);
void addConAffliction(int afflictingCon, int afflictingConOwner, Stat* afflictedStat, int edOwner);

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

        int binarySearchInt(int first, int last, int stat);
        int binarySearchFloat(int first, int last, int stat);
};

#endif // STATHOLDER_H
