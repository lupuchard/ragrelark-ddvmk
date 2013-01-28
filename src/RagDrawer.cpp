#include "RagDrawer.h"

RagDrawer::RagDrawer() {
    tileSize = 4;
    player = NULL;
    camX = 0;
    camY = 0;
}
RagDrawer::RagDrawer(int ts, Player* p) {
    tileSize = ts;
    player = p;
    camX = 0;
    camY = 0;
}

RagDrawer::~RagDrawer() {
    //dtor
}

void RagDrawer::drawTile(int x, int y, int z, Texture* tex, int loc) {
    tex->bind();

    int wid = tex->width / tileSize;
    int x1 = (loc % wid) * tileSize;
    int y1 = (loc / wid) * tileSize;
    double x1d = ((double)x1 / tex->width);
    double x2d = ((double)(x1 + tileSize) / tex->width);
    double y1d = ((double)y1 / tex->height);
    double y2d = ((double)(y1 + tileSize) / tex->height);

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
    tex->bind();

    int wid = tex->width / tileSize;
    int x1 = (loc % wid) * tileSize;
    int y1 = (loc / wid) * tileSize;
    double x1d = ((double)x1 / tex->width);
    double x2d = ((double)(x1 + tileSize) / tex->width);
    double y1d = ((double)y1 / tex->height);
    double y2d = ((double)(y1 + tileSize) / tex->height);

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
    tex->bind();

    double x1d = ((double)x1 / tex->width);
    double x2d = ((double)(x1 + size) / tex->width);
    double y1d = ((double)y1 / tex->height);
    double y2d = ((double)(y1 + size) / tex->height);

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
    tex->bind();

    double x1d = ((double)x1 / tex->width);
    double x2d = ((double)(x1 + wid1) / tex->width);
    double y1d = ((double)y1 / tex->height);
    double y2d = ((double)(y1 + hei1) / tex->height);

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
    tex->bind();

    double x1d = ((double)tx / tex->width);
    double x2d = ((double)(tx + wid) / tex->width);
    double y1d = ((double)ty / tex->height);
    double y2d = ((double)(ty + hei) / tex->height);

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

void RagDrawer::drawColorBox(int x1, int y1, int z, int x2, int y2, color c) {
    glColor4f(c.red / 255., c.green / 255., c.blue / 255., c.alpha / 255.);
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
    graphic g = unit->g;
    drawTile(x, y, Z_UNIT + y, getTexture(unit->g.tex), g.loc);
    int i = 0;
    if (unit == player->getUnit() || unit->equipment) {
        /*Item* items;
        int numItems;
        bool pl;
        if (unit == player->getUnit()) {
            items = primeFolder->getEquips()->getItems();
            numItems = primeFolder->getEquips()->getNumItems();
            pl = true;
        } else {
            items = unit->equipment->equips;
            numItems = unit->equipment->len;
            pl = false;
        }
        for (i = 0; i < numItems; i++) {
            ItemType* item = getItemType(items[i].itemType);
            bool gendered = false;
            int wid = 0;
            int hei = 0;
            switch(item->getEquipGType()) {
                case EQG_NONE: break;
                case EQG_NORM: wid = 32; hei = 32; break;
                case EQG_GNORM: wid = 32; hei = 32; gendered = true; break;
                case EQG_SMALL: wid = 16; hei = 16; break;
                case EQG_GSMALL: wid = 16; hei = 16; gendered = true; break;
                case EQG_TALL: wid = 16; hei = 32; break;
                case EQG_GTALL: wid = 16; hei = 32; gendered = true; break;
                case EQG_LONG: wid = 32; hei = 16; break;
                case EQG_GLONG: wid = 32; hei = 16; gendered = true; break;
                default: break;
            }
            if (wid) {
                int xMid = 0;
                int yMid = 0;
                int j;
                if (pl) j = i;
                else {
                    int typ = typeSlots[item->getType()];
                    switch(typ) {
                        case E_BHANDS: j = E_RHAND; break;
                        case E_BBHANDS: j = E_RHAND; break;
                        case E_RING: j = E_RING1; break;
                        default: j = typ;
                    }
                }
                switch(j) {
                    case E_HEAD: xMid = 16; yMid = 8; break;
                    case E_FACE: xMid = 16; yMid = 8; break;
                    case E_BACK: xMid = 16; yMid = 16; break;
                    case E_BAG: xMid = 16; yMid = 16; break;
                    case E_NECK: xMid = 16; yMid = 8; break;
                    case E_BODY: xMid = 16; yMid = 16; break;
                    case E_LHAND: xMid = 24; yMid = 16; break;
                    case E_RHAND: xMid = 8; yMid = 16; break;
                    case E_HANDS: xMid = 16; yMid = 16; break;
                    case E_WAIST: xMid = 16; yMid = 16; break;
                    case E_WRIST: xMid = 16; yMid = 16; break;
                    case E_FEET: xMid = 16; yMid = 24; break;
                    case E_RING1: xMid = 8; yMid = 16; break;
                    case E_RING2: xMid = 24; yMid = 16; break;
                    default: break;
                }
                int num = item->getEquipGLoc();
                //if (gendered) etc.
                int x0 = xMid - wid / 2 + x;
                int y0 = yMid - hei / 2 + y;
                int x1 = (num * 16) % playerTex->Width;
                int y1 = (num * 256) / playerTex->Width;
                ragd.drawTileSuperSpe(x0, y0, Z_UNIT + y + i + 1, wid, hei, playerTex, x1, y1, wid, hei);
            }
        }*/
    }
    int wid = max((int)((float)unit->getStatValue(S_HP) / unit->getStatValue(S_MAXHP) * tileSize), 3);
    if (wid < tileSize) {
        drawTileSuperSpe(x, y + tileSize - 4, Z_UNIT + y + i + 1, wid - 1, 4, getMenuTex(), 96, 0, wid - 1, 4);
        drawTileSuperSpe(x + wid - 1, y + tileSize - 4, Z_UNIT + y + i + 1, 2, 4, getMenuTex(), 126, 0, 2, 4);
    }
}

int RagDrawer::getTileSize() {
    return tileSize;
}

Player* RagDrawer::getPlayer() {
    return player;
}
