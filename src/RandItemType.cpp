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

#include "RandItemType.h"

RandItemType::RandItemType(unsigned short iBase, unsigned char minStack, unsigned char maxStack, unsigned int sWeight) {
    itemBase = iBase;
    min = minStack;
    max = maxStack;
    weight = sWeight;
}

Item RandItemType::genItem() const {
    unsigned short itemType = itemBase;
    short alt = ItemType::get(itemType)->getStatValue(Stat::ALT);
    while (alt) {
        if (rand() % 10 == 0) {
            itemType = alt;
            alt = ItemType::get(itemType)->getStatValue(Stat::ALT);
        } else alt = 0;
    }
    Item newItem = Item(itemType);
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
