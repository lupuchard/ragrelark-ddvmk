#ifndef MOBEQUIPSET_H
#define MOBEQUIPSET_H

#include <vector>
#include <stdlib.h>
#include <iostream>
using namespace std;

class MobEquipSet {
    public:
        MobEquipSet();

        int addEquips(int weight);
        void addEquipToEquips(int equips, int equip);

        int getEquips(unsigned int index, int* equips);
        int getRandEquips(int* equips);
        int getRandEquipsNear(unsigned int index, int* equips);
        int getRandIndex();
        int getNumEquipTypes();
    protected:
    private:
        vector<pair<int, vector<int> > > allEquips;
};

#endif // MOBEQUIPSET_H
