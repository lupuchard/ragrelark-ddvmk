#include "Location.h"
#include "Unit.h"

Unit* none = new Unit("not a unit", NULL);
vector<Item> genericBlankVector;

bool opens[] = {false, true, true, false, true, true, false, false, false, true, true, false, true, true, false, false,
            true, true, true, true, false, false, false, false, false, true, false, false, true, true, true, true};
bool cDoors[] = {true, false, false, true, false, false, false, true, true, false, false, true, false, false, true, true,
            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
bool oDoors[] = {false, true, false, false, true, false, false, false, false, true, false, false, true, false, false, false,
            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

Location::Location() {
    tile = 0;
    structure = 0;
    height = 0;
    light = 0;
    unit = none;
    items = &genericBlankVector;

    fog1 = FOG_NONE;
    fog2 = FOG_NONE;
    debris1 = 0;
    debris2 = 0;
}

Location::Location(int h) {
    tile = 0;
    structure = 0;
    height = h;
    light = 0;
    unit = none;
    items = &genericBlankVector;
}

void Location::clearStuff() {
    if (hasUnit()) {
        delete unit;
    }
    if (hasItems()) delete items;
}

bool Location::hasUnit() {
    return !(unit == none);
}

bool Location::hasItems() {
    return items != &genericBlankVector;
}

void Location::removeUnit() {
    unit = none;
}

bool Location::isOpen() {
    return opens[structure];
}

bool isOpen(int str) {
    return opens[str];
}

bool Location::isClosedDoor() {
    return cDoors[structure];
}

bool isClosedDoor(int str) {
    return cDoors[str];
}

bool Location::isOpenDoor() {
    return oDoors[structure];
}

bool isOpenDoor(int str) {
    return oDoors[str];
}

int Location::getTotalHeight() {
    if (structure == S_ROCK) return height + 8;
    return height;
}

//the return statement is for whether or not the item stacked. true = stacked, false = not stacked
bool Location::addItem(Item item) {
    if (items == &genericBlankVector) {
        items = new vector<Item>;
    } else {
        ItemType* thisItemType = getItemType(item.itemType);
        int stackLimit = typeStacks[thisItemType->getType()];
        if (stackLimit > 1) {
            for (unsigned int i = 0; i < items->size(); i++) {
                if ((*items)[i].itemType == item.itemType && (*items)[i].form == item.form && (*items)[i].quantityCharge < stackLimit) {
                    if ((*items)[i].quantityCharge == 0) (*items)[i].quantityCharge++;
                    if (item.quantityCharge == 0) item.quantityCharge++;
                    (*items)[i].quantityCharge += item.quantityCharge;
                    if ((*items)[i].quantityCharge > stackLimit) {
                        item.quantityCharge = (*items)[i].quantityCharge - stackLimit;
                        (*items)[i].quantityCharge = stackLimit;
                    } else {
                        return true;
                    }
                }
            }
        }
    }
    items->push_back(item);
    return false;
}

Item Location::removeItem(int itemI) {
    Item temp = (*items)[itemI];
    items->erase(items->begin() + itemI);
    if (items->size() == 0) {
        delete items;
        items = &genericBlankVector;
    }
    return temp;
}
