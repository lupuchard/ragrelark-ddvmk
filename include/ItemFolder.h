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

#ifndef ITEMFOLDER_H
#define ITEMFOLDER_H

#include "Item.h"
#include "EnvironmentManager.h"

class ItemFolder {
    public:
        ItemFolder();
        virtual ~ItemFolder();
        virtual int getNumItems() = 0;
        virtual Item* getItem(int index) = 0;
        virtual Item* getItems() = 0;
        virtual Item removeItem(int index) = 0;
        virtual bool addItem(Item* item) = 0;
    protected:
    private:
};

#endif // ITEMFOLDER_H
