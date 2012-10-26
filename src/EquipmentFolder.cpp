#include "EquipmentFolder.h"

Item emptySlots[NUM_EQUIP_SLOTS] = {Item(0), Item(1), Item(2), Item(3), Item(4),
                                    Item(5), Item(6), Item(7), Item(8), Item(9),
                                    Item(10), Item(11), Item(12), Item(12)};

/* headgear, face, back, bag, neck, body, lhand, rhand, hands, waist, wrist, feet, ring, ring */

EquipmentFolder::EquipmentFolder(): StatHolder(V_ITEM) {
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
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
    int typeSlot = typeSlots[itemTypeType];
    if (typeSlot >= 0 && typeSlot <= E_FEET) {
        //cout << "ab " << typeSlot << endl;
        returnItem = equipment[typeSlot];
        equipment[typeSlot] = item;
    } else if (typeSlot == E_RING) {
        if (equipment[E_RING1].itemType == emptySlots[E_RING1].itemType) {
            equipment[E_RING1] = item;
        } else {
            returnItem = equipment[E_RING2];
            equipment[E_RING2] = item;
        }
    }
    return returnItem;
}

short EquipmentFolder::getStatValue(int stat) {
    int value = 0;
    for (int i = 0; i < NUM_EQUIP_SLOTS; i++) {
        value += getItemType(equipment[i].itemType)->getStatValue(stat);
    }
    return value;
}

void EquipmentFolder::needToUpdate(int stat) {
    //nope
}
