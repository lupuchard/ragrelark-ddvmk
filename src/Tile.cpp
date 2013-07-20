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

#include <iostream>
#include "Tile.h"

Tile::Tile(String name, Graphic g, bool blockM, bool blockL, Tile* over): StatHolder(V_TILE) {
    graphic = g;

    blockMove = blockM;
    blockLight = blockL;

    index = -1;
    this->over = over;

    this->name = name;
}

Tile* Tile::getOver() {
    return over;
}

Graphic Tile::getGraphic() {
    return graphic;
}

bool Tile::blocksLight() {
    return blockLight;
}

bool Tile::blocksMove() {
    return blockMove;
}

unsigned short Tile::getIndex() {
    return index;
}

std::vector<Tile*> Tile::tiles;
std::map<String, Tile*> Tile::tileNameMap;
std::map<String, TileSet*> Tile::tileSets;

void Tile::add(Tile* tile) {
    tile->index = tiles.size();
    tiles.push_back(tile);
    tileNameMap[tile->name] = tile;
}
Tile* Tile::get(int index) {
    return tiles[index];
}
Tile* Tile::get(String n) {
    return tileNameMap[n];
}
bool Tile::has(String name) {
    return tileNameMap.find(name) != tileNameMap.end();
}
void Tile::parseSets(YAML::Node node) {
    TileSet* tileSet = new TileSet;
    tileSet->name = readYAMLStr(node, "Name", "nil", "Name expected.");
    YAML::Node tilesNode = node["Tiles"];
    if (tilesNode.IsSequence()) {
        for (YAML::const_iterator iter = tilesNode.begin(); iter != tilesNode.end(); ++iter) {
            String s = iter->as<String>();
            if (has(s)) {
                tileSet->tiles.push_back(get(s));
            } else std::cout << "'" << s << "' is not an existing tile.\n";
        }
        addSet(tileSet);
    } else std::cout << " err: Tiles expected.\n";
}
void Tile::addSet(TileSet* tileSet) {
    tileSets[tileSet->name] = tileSet;
}
TileSet* Tile::getSet(String name) {
    return tileSets[name];
}
bool Tile::hasSet(String name) {
    return tileSets.find(name) != tileSets.end();
}
void Tile::clean() {
    for (unsigned int i = 0; i < tiles.size(); i++) {
        delete tiles[i];
    }
    for (std::map<String, TileSet*>::iterator iter = tileSets.begin(); iter != tileSets.end(); ++iter) {
        delete iter->second;
    }
    tiles.clear();
    tileSets.clear();
}
