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

#ifndef PRIMEFOLDER_H
#define PRIMEFOLDER_H

#include "GroundFolder.h"
#include "EquipmentFolder.h"
#include "BagFolder.h"
#include "Texture.h"

class PrimeFolder: public ItemFolder {
    public:
        PrimeFolder();
        virtual ~PrimeFolder();
        void parseInv(YAML::Node node, std::ostream& lerr);
        int getNumItems();
        Item* getItem(int index);
        Item* getItems();
        Item removeItem(int index);
        bool addItem(Item* item);
        BagFolder* getBag();
        EquipmentFolder* getEquips();
        GroundFolder* getGround();
    protected:
    private:
        ItemType* parseOne(YAML::Node node, std::ostream& lerr);

        BagFolder* bag;
        EquipmentFolder* equips;
        GroundFolder* ground;
        Item allThem[3];
};

#endif // PRIMEFOLDER_H
