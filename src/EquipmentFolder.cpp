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

/* headgear, face, back, bag, neck, body, lhand, rhand, hands, waist, wrist, feet, ring, ring */

EquipmentFolder::EquipmentFolder(): StatHolder(V_ITEM) {
    equipment = new Item[ItemType::getNumSlots()];
    for (int i = 0; i < ItemType::getNumSlots(); i++) {
        equipment[i] = Item(ItemType::getEmptySlot(i));
    }
}

EquipmentFolder::~EquipmentFolder() {
    delete[] equipment;
}

int EquipmentFolder::getNumItems() {
    return ItemType::getNumSlots();
}
Item* EquipmentFolder::getItem(int index) {
    return &equipment[index];
}
Item* EquipmentFolder::getItems() {
    return equipment;
}
Item EquipmentFolder::removeItem(int index) {
    Item item = equipment[index];
    equipment[index] = Item(ItemType::getEmptySlot(index));
    return item;
}
bool EquipmentFolder::addItem(Item* item) {
    return false;
}
bool EquipmentFolder::equipItem(Item item, std::list<Item>& remove) {
    ItemType* type = item.getType();
    ItemSlot* slot = type->getSlot();
    if (!slot) return false;
    if (slot->quantity > 1) {
        bool success = false;
        for (int i = slot->index; i < slot->index + slot->quantity; i++) {
            if (equipment[i].getType() == ItemType::getEmptySlot(slot->index)) {
                equipment[i] = item;
                success = true;
            }
        }
        if (!success) {
            remove.push_back(equipment[slot->index]);
            equipment[slot->index] = item;
        }
    } else if (!slot->covers.empty()) {
        for (std::set<ItemSlot*>::iterator iter = slot->covers.begin(); iter != slot->covers.end(); ++iter) {
            ItemSlot* curSlot = *iter;
            if (equipment[curSlot->index].getType() != ItemType::getEmptySlot(curSlot->index) && equipment[curSlot->index].getType()->getSlot() == curSlot) {
                remove.push_back(equipment[curSlot->index]);
                equipment[curSlot->index] = Item(ItemType::getEmptySlot(curSlot->index));
            }
        }
        equipment[slot->index] = item;
    } else if (!slot->over.empty()) {
        for (std::set<ItemSlot*>::iterator iter = slot->over.begin(); iter != slot->over.end(); ++iter) {
            ItemSlot* curSlot = *iter;
            if (equipment[curSlot->index].getType() != ItemType::getEmptySlot(curSlot->index)) {
                remove.push_back(equipment[curSlot->index]);
                equipment[curSlot->index] = Item(ItemType::getEmptySlot(curSlot->index));
            }
        }
        equipment[slot->index] = item;
    } else {
        if (equipment[slot->index].getType() != ItemType::getEmptySlot(slot->index)) {
            remove.push_back(equipment[slot->index]);
        }
        equipment[slot->index] = item;
    }
    return true;
}

short EquipmentFolder::getStatValue(int stat) {
    int value = 0;
    for (int i = 0; i < ItemType::getNumSlots(); i++) {
        value += equipment[i].getType()->getStatValue(stat);
    }
    value += extra.getType()->getStatValue(stat);
    return value;
}
float EquipmentFolder::getStatValueF(int stat) {
    float value = 0;
    for (int i = 0; i < ItemType::getNumSlots(); i++) {
        value += equipment[i].getType()->getStatValueF(stat);
    }
    value += extra.getType()->getStatValueF(stat);
    return value;
}

void EquipmentFolder::needToUpdate(int stat) { }

void EquipmentFolder::setExtra(Item item) {
    extra = item;
}
void EquipmentFolder::removeExtra() {
    extra = Item(ItemType::getEmptySlot(0));
}

void EquipmentFolder::save(std::ostream& saveData) {
    for (int i = 0; i < getNumItems(); i++) {
        equipment[i].save(saveData);
    }
}

void EquipmentFolder::load(std::istream& saveData) {
    for (int i = 0; i < getNumItems(); i++) {
        equipment[i] = Item(saveData);
    }
}
