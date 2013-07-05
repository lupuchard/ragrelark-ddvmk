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

#include "EquipmentFolder.h"

Item emptySlots[HA_REAL_NUM_EQUIP_SLOTS] = {Item(0), Item(1), Item(2), Item(3), Item(4),
                                    Item(5), Item(6), Item(7), Item(8), Item(9),
                                    Item(10), Item(11), Item(12), Item(12), Item(0)};

/* headgear, face, back, bag, neck, body, lhand, rhand, hands, waist, wrist, feet, ring, ring */

EquipmentFolder::EquipmentFolder(): StatHolder(V_ITEM) {
    for (int i = 0; i < HA_REAL_NUM_EQUIP_SLOTS; i++) {
        equipment[i] = emptySlots[i];
    }
}

int EquipmentFolder::getNumItems() {
    return NUM_EQUIP_SLOTS;
}
Item* EquipmentFolder::getItem(int index) {
    return &equipment[index];
}
Item* EquipmentFolder::getItems() {
    return equipment;
}
Item EquipmentFolder::removeItem(int index) {
    Item item = equipment[index];
    equipment[index] = emptySlots[index];
    return item;
}
bool EquipmentFolder::addItem(Item* item) {
    return false;
}
Item EquipmentFolder::equipItem(Item item, int itemTypeType) {
    Item returnItem = emptySlots[0];
    int typeSlot = TYPE_SLOTS[itemTypeType];
    if (typeSlot >= 0 && typeSlot <= E_FEET) {
        if (typeSlot == E_LHAND) {
            if (TYPE_SLOTS[getItemType(equipment[E_RHAND].itemType)->getType()] == E_BHANDS) {
                returnItem = equipment[E_RHAND];
                equipment[E_RHAND] = emptySlots[E_RHAND];
            } else {
                returnItem = equipment[typeSlot];
            }
        } else {
            returnItem = equipment[typeSlot];
        }
        equipment[typeSlot] = item;
    } else if (typeSlot == E_RING) {
        if (equipment[E_RING1].itemType == emptySlots[E_RING1].itemType) {
            equipment[E_RING1] = item;
        } else {
            returnItem = equipment[E_RING2];
            equipment[E_RING2] = item;
        }
    } else if (typeSlot == E_BHANDS) {
        if (equipment[E_RHAND].itemType == emptySlots[E_RHAND].itemType) {
            returnItem = equipment[E_LHAND];
            equipment[E_LHAND] = emptySlots[E_LHAND];
            equipment[E_RHAND] = item;
        } else if (equipment[E_LHAND].itemType == emptySlots[E_LHAND].itemType) {
            returnItem = equipment[E_RHAND];
            equipment[E_RHAND] = item;
        } else {
            returnItem.quantityCharge = 2;
        }
    }
    return returnItem;
}

short EquipmentFolder::getStatValue(int stat) {
    int value = 0;
    for (int i = 0; i < HA_REAL_NUM_EQUIP_SLOTS; i++) {
        value += getItemType(equipment[i].itemType)->getStatValue(stat);
    }
    return value;
}
float EquipmentFolder::getStatValueF(int stat) {
    float value = 0;
    for (int i = 0; i < HA_REAL_NUM_EQUIP_SLOTS; i++) {
        value += getItemType(equipment[i].itemType)->getStatValueF(stat);
    }
    return value;
}

void EquipmentFolder::needToUpdate(int stat) {
    //nope
}
