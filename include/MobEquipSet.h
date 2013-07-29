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

#ifndef MOBEQUIPSET_H
#define MOBEQUIPSET_H

#include <vector>
#include <stdlib.h>
#include <iostream>
#include "ItemType.h"

class MobEquipSet {
    public:
        MobEquipSet();

        int addEquips(int weight);
        void addEquipToEquips(int equips, ItemType* equip);

        void getEquips(unsigned int index, std::vector<ItemType*>& equips);
        void getRandEquips(std::vector<ItemType*>& equips);
        void getRandEquipsNear(unsigned int index, std::vector<ItemType*>& equips);
        int getRandIndex();
        int getNumEquipTypes();

        //statics
        static void parseAll(YAML::Node, std::ostream& lerr);
        static MobEquipSet* get(String name);
        static bool has(String name);
        static void clear();
    private:
        std::vector<std::pair<int, std::vector<ItemType*> > > allEquips;

        //statics
        static std::map<String, MobEquipSet*> mobEquipSetNameMap;
};

#endif // MOBEQUIPSET_H
