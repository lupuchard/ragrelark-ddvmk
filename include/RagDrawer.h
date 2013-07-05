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

#ifndef RAGDRAWER_H
#define RAGDRAWER_H

#include "Texture.h"
#include "Player.h"
#include "Color.h"

#define Z_MENU 4000
#define Z_FOREGROUND 3000
#define Z_EFFECT 2200
#define Z_UNIT 70
#define Z_ITEM 40
#define Z_STRUCT 30
#define Z_SPLAT 20
#define Z_GROUND 10

//static const char X_DIRS[10] = {0, -1,  0,  1, -1,  0,  1, -1,  0,  1};
//static const char Y_DIRS[10] = {0,  1,  1,  1,  0,  0,  0, -1, -1, -1};

class RagDrawer {
    public:
        RagDrawer();
        RagDrawer(int tileSize, Player* player);
        virtual ~RagDrawer();

        /// Draws a tile from a texture.
        void drawTile(int x, int y, int z, Texture* tex, int loc);
        /// Draws a tile after rotating it rot degrees (multiple of 90).
        void drawTileRot(int x, int y, int z, Texture* tex, int loc, int rot, bool flip);
        /// Draws a more exact graphic from a texture.
        void drawTileSpe(int x, int y, int z, Texture* tex, int x1, int y1, int size);
        /// Even more parameters.
        void drawTileSuperSpe(int x, int y, int z, int wid, int hei, Texture* tex, int x1, int y1, int wid1, int hei1);
        /// More parameters again.
        void drawTileFull(int x, int y, int z, int wid, int hei, Texture* tex, int tx, int ty, int rot, bool flip);

        /// Draws a unit at a location.
        void drawUnit(int x, int y, Unit* unit);

        /// Draws a rectangle of a single color at a location.
        void drawColorBox(int x1, int y1, int z, int x2, int y2, Color c);

        Player* getPlayer();
        int getTileSize();

        double camX;
        double camY;
    protected:
    private:
        int tileSize;
        Player* player;
};

#endif // RAGDRAWER_H
