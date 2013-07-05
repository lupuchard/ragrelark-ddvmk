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
