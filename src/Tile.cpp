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
using namespace std;

vector<Tile*> tiles;
void addTile(Tile* tile) {
    tile->setIndex(tiles.size());
    tiles.push_back(tile);
}
Tile* getTile(int index) {
    return tiles[index];
}
void clearTiles() {
    for (unsigned int i = 0; i < tiles.size(); i++) {
        delete tiles[i];
    }
    tiles.clear();
}

Tile::Tile(int loc, int tex, int type, int border): StatHolder(V_TILE) {
    g.loc = loc;
    g.tex = tex;
    g.type = type;
    g.border = border;

    blockMove = false;
    blockLight = false;

    index = -1;
    over = NULL;
}

graphic Tile::getGraphic() {
    return g;
}

void Tile::setOver(Tile* t) {
    over = t;
}

Tile* Tile::getOver() {
    return over;
}

void Tile::setBlock(bool blockM, bool blockL) {
    blockLight = blockL;
    blockMove = blockM;
}

bool Tile::blocksLight() {
    return blockLight;
}

bool Tile::blocksMove() {
    return blockMove;
}

void Tile::setIndex(int i) {
    index = i;
}

unsigned short Tile::getIndex() {
    return index;
}
