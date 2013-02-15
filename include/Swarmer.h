#ifndef SWARMER_H
#define SWARMER_H

#include "Unit.h"

class Swarmer: public Unit {
    public:
        Swarmer(string n, StatHolder* prototype);
        virtual ~Swarmer();

        short getStatValue(int stat);
        short modifyStat(int stat, int amount);
        float getStatValueF(int stat);
        float modifyStatF(int stat, float amount);

        int howMany();

        void add(Swarmer* unit);
        Unit* take(int unit);
    private:
        vector<Swarmer*> others;
};

#endif // SWARM_H
