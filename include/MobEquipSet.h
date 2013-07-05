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

class MobEquipSet {
    public:
        MobEquipSet();

        int addEquips(int weight);
        void addEquipToEquips(int equips, int equip);

        int getEquips(unsigned int index, int* equips);
        int getRandEquips(int* equips);
        int getRandEquipsNear(unsigned int index, int* equips);
        int getRandIndex();
        int getNumEquipTypes();
    protected:
    private:
        std::vector<std::pair<int, std::vector<int> > > allEquips;
};

#endif // MOBEQUIPSET_H
