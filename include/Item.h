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

#ifndef ITEM_H
#define ITEM_H

#include "ItemType.h"
#define I_NONE 0
#define I_SLOT 3
#define I_GFOLDER 11
#define I_EFOLDER 12
#define I_BFOLDER 13
#define I_PFOLDER 14
#define I_BAG 23
#define I_FOOD 41

struct Item {
    public:
        Item();
        Item(int itemType);
        unsigned short itemType;
        unsigned char quantityCharge;
        unsigned char form;
    protected:
    private:
};

#endif // ITEM_H
