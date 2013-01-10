#include "RandItemType.h"

RandItemType::RandItemType(unsigned short iBase, unsigned char minStack, unsigned char maxStack, unsigned int sWeight) {
    itemBase = iBase;
    min = minStack;
    max = maxStack;
    weight = sWeight;
}

Item RandItemType::genItem() const {
    Item newItem = Item(itemBase);
    if (max > 1) {
        if (max > min) {
            int num = rand() % (max - min + 1) + min;
            newItem.quantityCharge = num;
        } else {
            newItem.quantityCharge = min;
        }
    }
    return newItem;
}

unsigned short RandItemType::getItemBase() const {
    return itemBase;
}

unsigned char RandItemType::getMax() const {
    return max;
}

unsigned char RandItemType::getMin() const {
    return min;
}

unsigned int RandItemType::getWeight() const {
    return weight;
}
