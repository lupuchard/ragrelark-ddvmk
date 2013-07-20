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
#include "Texture.h"

#ifndef TILE_H
#define TILE_H

enum TileType{TT_NORMAL, TT_SMOOTH, TT_SMOOTHUP, TT_SMOOTHDOWN, TT_LONGHORZ = 11};

struct Graphic {
    unsigned short loc;
    unsigned char type;
    unsigned char border;
    Texture* tex;

    Graphic(): loc(0), type(0), border(0), tex(NULL) {}
};

class Tile;
struct TileSet {
    String name;
    std::vector<Tile*> tiles;
};

class Tile : public StatHolder {
    public:
        Tile(String name, Graphic g, bool blockM = false, bool blockL = false, Tile* over = NULL);

        Graphic getGraphic();
        /// the tile that this one renders over, and therefore must render under this one
        Tile* getOver();
        /// true = it blocks light, false = it doesnt
        bool blocksLight();
        /// true = it blocks movement, false = it doesnt
        bool blocksMove();

        unsigned short getIndex();

        //statics
        static void add(Tile* tile);
        static Tile* get(int index);
        static Tile* get(String name);
        static bool has(String name);
        static void parseSets(YAML::Node node);
        static void addSet(TileSet* tileSet);
        static TileSet* getSet(String name);
        static bool hasSet(String name);
        static void clean();
    private:
        Graphic graphic;
        Tile* over;
        bool blockLight;
        bool blockMove;
        unsigned short index;
        String name;

        static std::vector<Tile*> tiles;
        static std::map<String, Tile*> tileNameMap;
        static std::map<String, TileSet*> tileSets;
};

#endif // TILE_H
