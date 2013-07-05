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

#ifndef ENVIRONMENTMANAGER_H
#define ENVIRONMENTMANAGER_H

#include "Zone.h"

class EnvironmentManager {
    public:
        EnvironmentManager();
        virtual ~EnvironmentManager();
        virtual void addItemToPlace(Coord pos, Zone* z, Item item) = 0;
        virtual Item removeItemFromPlace(Coord pos, Zone* z, int index) = 0;
    protected:
    private:
};

#endif // ENVIRONMENTMANAGER_H
