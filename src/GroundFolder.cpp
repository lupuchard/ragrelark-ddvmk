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

#include "GroundFolder.h"

EnvironmentManager* em;
void setEnvironmentManager(EnvironmentManager* ev) {
    em = ev;
}

GroundFolder::GroundFolder() {
    zone = NULL;
}
GroundFolder::~GroundFolder() {}

int GroundFolder::getNumItems() {
    Location* l = zone->getLocationAt(loc);
    if (l->items) return l->items->size();
    return 0;
}
Item* GroundFolder::getItem(int index) {
    return &zone->getLocationAt(loc)->items->at(index);
}
Item* GroundFolder::getItems() {
    return &zone->getLocationAt(loc)->items->at(0);
}
Item GroundFolder::removeItem(int index) {
    return em->removeItemFromPlace(loc, zone, index);
}

void GroundFolder::setLocation(Zone* z, Coord c) {
    zone = z;
    loc = c;
}

bool GroundFolder::addItem(Item* item) {
    em->addItemToPlace(loc, zone, *item);
    return true;
}
