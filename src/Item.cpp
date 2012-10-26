#include "Item.h"

Item::Item() {
    quantityCharge = 0;
    form = 0;
}

Item::Item(int iType) {
    itemType = (unsigned short)iType;
    quantityCharge = 0;
    form = 0;
}
