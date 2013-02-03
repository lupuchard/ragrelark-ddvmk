#ifndef EQUIPMENTFOLDER_H
#define EQUIPMENTFOLDER_H

#include "ItemFolder.h"
#define NUM_EQUIP_SLOTS 14

#define HA_REAL_NUM_EQUIP_SLOTS 15

#define SECRET_AMMO_INDEX 15

class EquipmentFolder : public ItemFolder, public StatHolder {
    public:
        EquipmentFolder();
        int getNumItems();
        Item* getItem(int index);
        Item* getItems();
        Item removeItem(int index);
        bool addItem(Item* item);
        Item equipItem(Item item, int itemTypeType);

        short getStatValue(int stat);
        float getStatValueF(int stat);
        void needToUpdate(int stat);
    protected:
    private:
        Item equipment[HA_REAL_NUM_EQUIP_SLOTS];
};

#endif // EQUIPMENTFOLDER_H
