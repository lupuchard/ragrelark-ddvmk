#include "Formula.h"
#include <string>
using namespace std;

#ifndef STAT_H
#define STAT_H

/* This defines a type of statistic. */
class Stat {
    public:
        /* No two stats of the same isFloat status can have the same index. */
        Stat(string name, Formula* formula, unsigned char index, bool isFloat);
        virtual ~Stat();

        /* This is mostly used for parsing, but may later be used in-game. */
        string getName();

        /* The formula that defines the statistic. A formula with a length of 1 (assumed to be either SLF or some constant) will never be recalled. */
        Formula* getFormula();

        bool isItFloat();
        unsigned char getIndex();
    protected:
    private:
        Formula* formula;
        string name;
        bool isFloat;
        unsigned char index;
};

Stat* getStat(VOwner type, int statI);
int getNumStats(VOwner type);
int addStat(VOwner type, Stat* theStat);

#endif // STAT_H
