#ifndef RANDITEMTYPE_H
#define RANDITEMTYPE_H

#include "Item.h"

class RandItemType {
    public:
        RandItemType(unsigned short iBase, unsigned char minStack, unsigned char maxStack, unsigned int weight);
        Item genItem() const;

        unsigned short getItemBase() const;
        unsigned int getWeight() const;
        unsigned char getMin() const;
        unsigned char getMax() const;
    protected:
    private:
        unsigned short itemBase;
        unsigned int weight;
        unsigned char min;
        unsigned char max;
};

#endif // RANDITEMTYPE_H
