/*
 *  Copyright 2013 Luke Puchner-Hardman
 *
 *  This file is part of Ragrelark.
 *  Ragrelark is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ragrelark is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ragrelark.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Location.h"
#include "Unit.h"

Unit* none = new Unit("not a unit", NULL);

bool OPENS[] = {false, true, true, false, true, true, false, false, false, true, true, false, true, true, false, false,
            true, true, true, true, false, false, false, false, false, true, false, false, true, true, true, true};
bool CDOORS[] = {true, false, false, true, false, false, false, true, true, false, false, true, false, false, true, true,
            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
bool ODOORS[] = {false, true, false, false, true, false, false, false, false, true, false, false, true, false, false, false,
            false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

Location::Location() {
    init();
}

Location::Location(int h) {
    init();
    height = h;
}

void Location::init() {
    tile = 0;
    structure = 0;
    height = 0;
    light = 0;
    unit = none;
    items = NULL;
    fog1 = fog2 = FOG_NONE;
    debris1 = debris2 = 0;
}

void Location::clearStuff() {
    if (hasUnit()) delete unit;
    if (hasItems()) delete items;
}

bool Location::hasUnit() {
    return !(unit == none);
}

bool Location::hasItems() {
    return items;
}

void Location::removeUnit() {
    unit = none;
}

bool Location::isOpen() {
    return OPENS[structure];
}

bool isOpen(int str) {
    return OPENS[str];
}

bool Location::isClosedDoor() {
    return CDOORS[structure];
}

bool isClosedDoor(int str) {
    return CDOORS[str];
}

bool Location::isOpenDoor() {
    return ODOORS[structure];
}

bool isOpenDoor(int str) {
    return ODOORS[str];
}

int Location::getTotalHeight() {
    if (structure == S_ROCK) return height + 8;
    return height;
}

//the return statement is for whether or not the item stacked. true = stacked, false = not stacked, fileNotFound = destacked
bool Location::addItem(Item item) {
    if (!items) {
        items = new std::vector<Item>;
    } else {
        ItemType* thisItemType = item.getType();
        int stackLimit = thisItemType->getStack();
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
    if (items->empty()) {
        delete items;
        items = NULL;
    }
    return temp;
}
