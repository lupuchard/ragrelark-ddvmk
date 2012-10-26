#include "BagFolder.h"

BagFolder::BagFolder(int s) {
    size = s;
    items.reserve(size);
}

int BagFolder::getNumItems() {
    return items.size();
}

Item* BagFolder::getItem(int index) {
    return &items[index];
}

Item* BagFolder::getItems() {
    return &items[0];
}

Item BagFolder::removeItem(int index) {
    Item item = items[index];
    items.erase(items.begin() + index);
    return item;
}

bool BagFolder::addItem(Item* item) {
    int itemTypeType = getItemType(item->itemType)->getType();
    int stackAmount = typeStacks[itemTypeType];
    if (stackAmount > 1) {
        for (unsigned int i = 0; i < items.size(); i++) {
            Item* thisItem = &items[i];
            if (thisItem->itemType == item->itemType && thisItem->form == item->form && thisItem->quantityCharge < stackAmount) {
                if (thisItem->quantityCharge == 0) thisItem->quantityCharge++;
                if (item->quantityCharge == 0) item->quantityCharge++;
                thisItem->quantityCharge += item->quantityCharge;
                if (thisItem->quantityCharge > stackAmount) {
                    item->quantityCharge = thisItem->quantityCharge - stackAmount;
                    thisItem->quantityCharge = stackAmount;
                    //items.push_back(*item);
                } else {
                    return true;
                }
            }
        }
    }
    if (items.size() < size) {
        items.push_back(*item);
        return true;
    }
    return false;
}

int BagFolder::getCapacity() {
    return size;
}

void BagFolder::setCapacity(int c) {
    size = c;
}
