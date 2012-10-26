#ifndef ITEM_H
#define ITEM_H

#include "ItemType.h"
#define I_NONE 0
#define I_SLOT 3
#define I_GFOLDER 11
#define I_EFOLDER 12
#define I_BFOLDER 13
#define I_PFOLDER 14
#define I_BAG 23
#define I_FOOD 41

class Item
{
    public:
        Item();
        Item(int itemType);
        unsigned short itemType;
        unsigned char quantityCharge;
        unsigned char form;
    protected:
    private:
};

#endif // ITEM_H
