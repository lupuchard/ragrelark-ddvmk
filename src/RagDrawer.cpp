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

#include "RagDrawer.h"

RagDrawer::RagDrawer() {
    tileSize = 4;
    player = NULL;
    camX = 0;
    camY = 0;
    utilTex = NULL;
}
RagDrawer::RagDrawer(int ts, Player* p, Texture* utilTex) {
    tileSize = ts;
    player = p;
    camX = 0;
    camY = 0;
    this->utilTex = utilTex;
}

RagDrawer::~RagDrawer() {
    //dtor
}

void RagDrawer::drawTile(int x, int y, int z, Texture* tex, int loc) {
    tex->gl();

    int wid = tex->getWidth() / tileSize;
    int x1 = (loc % wid) * tileSize;
    int y1 = (loc / wid) * tileSize;
    double x1d = ((double)x1 / tex->getWidth());
    double x2d = ((double)(x1 + tileSize) / tex->getWidth());
    double y1d = ((double)y1 / tex->getHeight());
    double y2d = ((double)(y1 + tileSize) / tex->getHeight());

    glBegin(GL_QUADS);
    glTexCoord2f(x1d, y1d);
    glVertex3i(x, y, z);
    glTexCoord2f(x2d, y1d);
    glVertex3i(x + tileSize, y, z);
    glTexCoord2f(x2d, y2d);
    glVertex3i(x + tileSize, y + tileSize, z);
    glTexCoord2f(x1d, y2d);
    glVertex3i(x, y + tileSize, z);
    glEnd();
}

//rot: 0 = 0 deg, 1 = 90 deg, 2 = 180 deg, 3 = 270 deg
void RagDrawer::drawTileRot(int x, int y, int z, Texture* tex, int loc, int rot, bool flip) {
    tex->gl();

    int wid = tex->getWidth() / tileSize;
    int x1 = (loc % wid) * tileSize;
    int y1 = (loc / wid) * tileSize;
    double x1d = ((double)x1 / tex->getWidth());
    double x2d = ((double)(x1 + tileSize) / tex->getWidth());
    double y1d = ((double)y1 / tex->getHeight());
    double y2d = ((double)(y1 + tileSize) / tex->getHeight());

    int ix[] = {x, x + tileSize, x + tileSize, x};
    int iy[] = {y, y, y + tileSize, y + tileSize};
    if (flip) {
        ix[0] = x + tileSize;
        ix[1] = x; ix[2] = x;
        ix[3] = x + tileSize;
    }
    while(rot > 0) {
        rot--;
        int temp1 = ix[0];
        int temp2 = iy[0];
        for (int i = 0; i < 3; i++) {
            ix[i] = ix[i + 1];
            iy[i] = iy[i + 1];
        }
        ix[3] = temp1;
        iy[3] = temp2;
    }

    glBegin(GL_QUADS);
    glTexCoord2f(x1d, y1d);
    glVertex3i(ix[0], iy[0], z);
    glTexCoord2f(x2d, y1d);
    glVertex3i(ix[1], iy[1], z);
    glTexCoord2f(x2d, y2d);
    glVertex3i(ix[2], iy[2], z);
    glTexCoord2f(x1d, y2d);
    glVertex3i(ix[3], iy[3], z);
    glEnd();
}

void RagDrawer::drawTileSpe(int x, int y, int z, Texture* tex, int x1, int y1, int size) {
    tex->gl();

    double x1d = ((double)x1 / tex->getWidth());
    double x2d = ((double)(x1 + size) / tex->getWidth());
    double y1d = ((double)y1 / tex->getHeight());
    double y2d = ((double)(y1 + size) / tex->getHeight());

    glBegin(GL_QUADS);
    glTexCoord2f(x1d, y1d);
    glVertex3i(x, y, z);
    glTexCoord2f(x2d, y1d);
    glVertex3i(x + size, y, z);
    glTexCoord2f(x2d, y2d);
    glVertex3i(x + size, y + size, z);
    glTexCoord2f(x1d, y2d);
    glVertex3i(x, y + size, z);
    glEnd();
}

void RagDrawer::drawTileSuperSpe(int x, int y, int z, int wid, int hei, Texture* tex, int x1, int y1, int wid1, int hei1) {
    tex->gl();

    double x1d = ((double)x1 / tex->getWidth());
    double x2d = ((double)(x1 + wid1) / tex->getWidth());
    double y1d = ((double)y1 / tex->getHeight());
    double y2d = ((double)(y1 + hei1) / tex->getHeight());

    glBegin(GL_QUADS);
    glTexCoord2f(x1d, y1d);
    glVertex3i(x, y, z);
    glTexCoord2f(x2d, y1d);
    glVertex3i(x + wid, y, z);
    glTexCoord2f(x2d, y2d);
    glVertex3i(x + wid, y + hei, z);
    glTexCoord2f(x1d, y2d);
    glVertex3i(x, y + hei, z);
    glEnd();
}

void RagDrawer::drawTileFull(int x, int y, int z, int wid, int hei, Texture* tex, int tx, int ty, int rot, bool flip) {
    tex->gl();

    double x1d = ((double)tx / tex->getWidth());
    double x2d = ((double)(tx + wid) / tex->getWidth());
    double y1d = ((double)ty / tex->getHeight());
    double y2d = ((double)(ty + hei) / tex->getHeight());

    int ix[] = {x, x + wid, x + wid, x};
    int iy[] = {y, y, y + hei, y + hei};
    if (flip) {
        ix[0] = x + wid;
        ix[1] = x; ix[2] = x;
        ix[3] = x + wid;
    }
    while(rot > 0) {
        rot--;
        int temp1 = ix[0];
        int temp2 = iy[0];
        for (int i = 0; i < 3; i++) {
            ix[i] = ix[i + 1];
            iy[i] = iy[i + 1];
        }
        ix[3] = temp1;
        iy[3] = temp2;
    }

    glBegin(GL_QUADS);
    glTexCoord2f(x1d, y1d);
    glVertex3i(ix[0], iy[0], z);
    glTexCoord2f(x2d, y1d);
    glVertex3i(ix[1], iy[1], z);
    glTexCoord2f(x2d, y2d);
    glVertex3i(ix[2], iy[2], z);
    glTexCoord2f(x1d, y2d);
    glVertex3i(ix[3], iy[3], z);
    glEnd();
}

void RagDrawer::drawColorBox(int x1, int y1, int z, int x2, int y2, Color c) {
    c.gl();
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex3i(x1, y1, z);
    glVertex3i(x2, y1, z);
    glVertex3i(x2, y2, z);
    glVertex3i(x1, y2, z);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void RagDrawer::drawUnit(int x, int y, Unit* unit) {
    Graphic g = unit->graphic;
    drawTile(x, y, Z_UNIT + y, g.tex, g.loc);
    int i = 0;
    if (unit == player->getUnit() || unit->equipment) {
        Item* items;
        int numItems;
        if (unit == player->getUnit()) {
            items = player->getPrimeFolder()->getEquips()->getItems();
            numItems = player->getPrimeFolder()->getEquips()->getNumItems();
        } else {
            items = unit->equipment->equips;
            numItems = unit->equipment->len;
        }
        for (i = 0; i < numItems; i++) {
            ItemType* item = items[i].getType();
            if (!item->getSlot()) continue;
            Coord mid = item->getSlot()->onBody;
            if (mid == ORIGIN) continue;
            int wid = 0;
            int hei = 0;
            Graphic g = item->getEquippedGraphic();
            switch(g.type) {
                case EQG_NONE: continue;
                case EQG_LARGE: wid = TILE_SIZE; hei = TILE_SIZE; break;
                case EQG_SMALL: wid = TILE_SIZE / 2; hei = TILE_SIZE / 2; break;
                case EQG_TALL: wid = TILE_SIZE / 2; hei = TILE_SIZE; break;
                case EQG_LONG: wid = TILE_SIZE; hei = TILE_SIZE / 2; break;
                default: continue;
            }
            int x0 = mid.x - wid / 2 + x;
            int y0 = mid.y - hei / 2 + y;
            int x1 = g.loc % (TEX_TILE_WIDTH * 2) * (TILE_SIZE / 2);
            int y1 = g.loc / (TEX_TILE_WIDTH * 2) * (TILE_SIZE / 2);
            drawTileSuperSpe(x0, y0, Z_UNIT + y + i + 1, wid, hei, g.tex, x1, y1, wid, hei);
        }
    } // TODO equip graphics
    int wid = std::max((int)((float)unit->getStatValue(Stat::HP) / unit->getStatValue(Stat::MAXHP) * tileSize), 3);
    if (wid < tileSize) {
        drawTileSuperSpe(x, y + tileSize - 4, Z_UNIT + y + i + 1, wid - 1, 4, utilTex, 96, 0, wid - 1, 4);
        drawTileSuperSpe(x + wid - 1, y + tileSize - 4, Z_UNIT + y + i + 1, 2, 4, utilTex, 126, 0, 2, 4);
    }
}

int RagDrawer::getTileSize() {
    return tileSize;
}

Player* RagDrawer::getPlayer() {
    return player;
}
