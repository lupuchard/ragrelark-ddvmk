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

#include <string>
#include "StatHolder.h"

#ifndef TILE_H
#define TILE_H

enum TileType{TT_NORMAL, TT_SMOOTH, TT_SMOOTHUP, TT_SMOOTHDOWN, TT_LONGHORZ = 11, TT_OVER = 20};

typedef struct {
    unsigned char loc;
    unsigned char tex;
    unsigned char type;
    unsigned char border;
} graphic;

class Tile : public StatHolder {
    public:
        Tile(int loc, int tex, int type, int border);
        void setOver(Tile* t);
        void setBlock(bool blockM, bool blockL);

        graphic getGraphic();
        Tile* getOver();
        bool blocksLight();
        bool blocksMove();

        void setIndex(int index);
        unsigned short getIndex();
    protected:
    private:
        graphic g;
        Tile* over;
        bool blockLight;
        bool blockMove;
        unsigned short index;
};

void addTile(Tile* tile);
Tile* getTile(int index);
void clearTiles();

#endif // TILE_H
