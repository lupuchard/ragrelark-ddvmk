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
    bag = new BagFolder(50);
}

PrimeFolder::~PrimeFolder() {
    delete bag;
    delete equips;
    delete ground;
}

void PrimeFolder::parseInv(YAML::Node node, std::ostream& lerr) {
    allThem[0] = Item(parseOne(node["Bag"], lerr));
    allThem[1] = Item(parseOne(node["Equipment"], lerr));
    allThem[2] = Item(parseOne(node["Ground"], lerr));
}

ItemType* PrimeFolder::parseOne(YAML::Node node, std::ostream& lerr) {
    Graphic g;
    g.tex = Texture::get(node["Texture"].as<String>());
    g.loc = readYAMLCoord(node, "Tile", ORIGIN, "Tile loc expected in inventory folder.", lerr).index(TEX_TILE_WIDTH);
    ItemType* newItemType = new ItemType(node["Name"].as<String>(), "", g, ItemType::getTypeI(node["Type"].as<String>()));
    return newItemType;
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
