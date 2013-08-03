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

#include "Item.h"

Item::Item() {
    itemType = 0;
    quantityCharge = 0;
    form = 0;
}

Item::Item(int iType) {
    itemType = (unsigned short)iType;
    quantityCharge = 0;
    form = 0;
}
Item::Item(ItemType* itemType) {
    this->itemType = itemType->getIndex();
    quantityCharge = 0;
    form = 0;
}

ItemType* Item::getType() {
    return ItemType::get(itemType);
}

String Item::getName() {
    if (quantityCharge > 1 && getType()->getStack() > 1) {
        return its(quantityCharge) + " " + pluralize(getType()->getName());
    } else return article(getType()->getName()) + " " + getType()->getName();
}

void Item::save(std::ostream& saveData) {
    outSht(itemType, saveData);
    saveData.put(form);
    saveData.put(quantityCharge);
}
Item::Item(std::istream& saveData) {
    itemType = inSht(saveData);
    form = saveData.get();
    quantityCharge = saveData.get();
}
