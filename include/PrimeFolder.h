#ifndef PRIMEFOLDER_H
#define PRIMEFOLDER_H

#include "GroundFolder.h"
#include "EquipmentFolder.h"
#include "BagFolder.h"

class PrimeFolder: public ItemFolder {
    public:
        PrimeFolder();
        virtual ~PrimeFolder();
        int getNumItems();
        Item* getItem(int index);
        Item* getItems();
        Item removeItem(int index);
        bool addItem(Item* item);
        BagFolder* getBag();
        EquipmentFolder* getEquips();
        GroundFolder* getGround();
        //void setBag(BagFolder* b);
    protected:
    private:
        BagFolder* bag;
        EquipmentFolder* equips;
        GroundFolder* ground;
        Item allThem[3];
};

#endif // PRIMEFOLDER_H
