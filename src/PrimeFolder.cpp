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

#include "PrimeFolder.h"

PrimeFolder::PrimeFolder() {
    equips = new EquipmentFolder();
    ground = new GroundFolder();
    bag = new BagFolder(5);
    allThem[0] = Item(15);
    allThem[1] = Item(14);
    allThem[2] = Item(13);
}

PrimeFolder::~PrimeFolder() {
    delete bag;
    delete equips;
    delete ground;
}

int PrimeFolder::getNumItems() {
    return 3;
}

Item* PrimeFolder::getItem(int index) {
    return &allThem[index];
}

Item* PrimeFolder::getItems() {
    return allThem;
}

Item PrimeFolder::removeItem(int index) {
    return allThem[0];
}

bool PrimeFolder::addItem(Item* item) {
    return false;
}

BagFolder* PrimeFolder::getBag() {
    return bag;
}

EquipmentFolder* PrimeFolder::getEquips() {
    return equips;
}

GroundFolder* PrimeFolder::getGround() {
    return ground;
}

/*void PrimeFolder::setBag(BagFolder* b) {
    bag = b;
}*/
