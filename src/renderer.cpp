//#include <GL/gl.h>
//#include <GL/glu.h>
//#include <GL/glext.h>
//#include <SDL/SDL.h>

#include "Start.h"
#include "graphics.h"

#define IM_WID 200
#define INTERVAL_MAX 360
#define MAX_MENU_ITEMS 12

const string menuActionNames[] = {"(R)ead", "E(x)amine", "To ba(G)", "(d)rop", "E(Q)uip", "(e)at", "(R)ead", "E(x)amine"};

const char arrowX[] = {0, 16, 32, 48, 64,  0, 16, 32, 48, 64};
const char arrowY[] = {0,  0,  0,  0,  0, 16, 16, 16, 16, 16};

//miss = charcoal, scrape = salmon, hit = maroon, crit = red, megacrit = crimson
int interval = 0;

int curArrowY = WIN1_HEIGHT / 2;
int selectedShift = 0;

map<int, pair<string, color> > statusies;

void Start::addStatus(string name, color c, int type) {
    statusies[type] = pair<string, color>(name, c);
}
void Start::removeStatus(int type) {
    statusies.erase(type);
}

color Start::dark(color c) {
    c.red   /= 2;
    c.green /= 2;
    c.blue  /= 2;
    return c;
}
color Start::light(color c) {
    c.red   = (256 - c.red)   / 2 + c.red;
    c.green = (256 - c.green) / 2 + c.green;
    c.blue  = (256 - c.blue)  / 2 + c.blue;
    return c;
}

void Start::drawTile(int x, int y, int z, Texture* tex, int loc) {
    tex->Bind();

    int wid = tex->Width / TILE_SIZE;
    int x1 = (loc % wid) * TILE_SIZE;
    int y1 = (loc / wid) * TILE_SIZE;
    double x1d = ((double)x1 / tex->Width);
    double x2d = ((double)(x1 + TILE_SIZE) / tex->Width);
    double y1d = ((double)y1 / tex->Height);
    double y2d = ((double)(y1 + TILE_SIZE) / tex->Height);

    glBegin(GL_QUADS);
    glTexCoord2f(x1d, y1d);
    glVertex3i(x, y, z);
    glTexCoord2f(x2d, y1d);
    glVertex3i(x + TILE_SIZE, y, z);
    glTexCoord2f(x2d, y2d);
    glVertex3i(x + TILE_SIZE, y + TILE_SIZE, z);
    glTexCoord2f(x1d, y2d);
    glVertex3i(x, y + TILE_SIZE, z);
    glEnd();
}


//rot: 0 = 0 deg, 1 = 90 deg, 2 = 180 deg, 3 = 270 deg
void Start::drawTileRot(int x, int y, int z, Texture* tex, int loc, int rot, bool flip) {
    tex->Bind();

    int wid = tex->Width / TILE_SIZE;
    int x1 = (loc % wid) * TILE_SIZE;
    int y1 = (loc / wid) * TILE_SIZE;
    double x1d = ((double)x1 / tex->Width);
    double x2d = ((double)(x1 + TILE_SIZE) / tex->Width);
    double y1d = ((double)y1 / tex->Height);
    double y2d = ((double)(y1 + TILE_SIZE) / tex->Height);

    int ix[] = {x, x + TILE_SIZE, x + TILE_SIZE, x};
    int iy[] = {y, y, y + TILE_SIZE, y + TILE_SIZE};
    if (flip) {
        ix[0] = x + TILE_SIZE;
        ix[1] = x; ix[2] = x;
        ix[3] = x + TILE_SIZE;
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

void Start::drawTileSpe(int x, int y, int z, Texture* tex, int x1, int y1, int size) {
    tex->Bind();

    double x1d = ((double)x1 / tex->Width);
    double x2d = ((double)(x1 + size) / tex->Width);
    double y1d = ((double)y1 / tex->Height);
    double y2d = ((double)(y1 + size) / tex->Height);

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

void Start::drawTileSuperSpe(int x, int y, int z, int wid, int hei, Texture* tex, int x1, int y1, int wid1, int hei1) {
    tex->Bind();

    double x1d = ((double)x1 / tex->Width);
    double x2d = ((double)(x1 + wid1) / tex->Width);
    double y1d = ((double)y1 / tex->Height);
    double y2d = ((double)(y1 + hei1) / tex->Height);

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

void Start::drawTileFull(int x, int y, int z, int wid, int hei, Texture* tex, int tx, int ty, int rot, bool flip) {
    tex->Bind();

    double x1d = ((double)tx / tex->Width);
    double x2d = ((double)(tx + wid) / tex->Width);
    double y1d = ((double)ty / tex->Height);
    double y2d = ((double)(ty + hei) / tex->Height);

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

void drawColorBox(int x1, int y1, int z, int x2, int y2, color c) {
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

void Start::drawUnit(int x, int y, Unit* unit) {
    graphic g = unit->getGraphic();
    drawTile(x, y, Z_UNIT + y, getTexture(unit->getGraphic().tex), g.loc);
    int wid = max((int)((float)unit->getStatValue(S_HP) / unit->getStatValue(S_MAXHP) * TILE_SIZE), 3);
    if (wid < TILE_SIZE) {
        drawTileSuperSpe(x, y + TILE_SIZE - 4, Z_UNIT + y + 1, wid - 1, 4, menuTex, 96, 0, wid - 1, 4);
        drawTileSuperSpe(x + wid - 1, y + TILE_SIZE - 4, Z_UNIT + y + 1, 2, 4, menuTex, 126, 0, 2, 4);
    }
}

void Start::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    renderGround();

    glPopMatrix();

    renderMenu();
    renderMessages();
    renderBars();

    glFlush();
    SDL_GL_SwapBuffers();

    interval++;
    if (interval == INTERVAL_MAX) {
        interval = 0;
    }
    frameTime++;
}

void Start::renderBars() {
    Unit* pUnit = player->getUnit();
    int hp  = pUnit->getStatValue(S_HP);
    int mhp = pUnit->getStatValue(S_MAXHP);
    int mp  = pUnit->getStatValue(S_MANA);
    int mmp = pUnit->getStatValue(S_MAXMANA);
    int exp = pUnit->getStatValue(S_EXP);
    int expReq = pUnit->getStatValue(S_EXPREQ);
    float hun = (float)pUnit->getStatValue(S_HUNGER);
    float sta = (float)pUnit->getStatValue(S_STAMINA);
    float percents[] = {(float)hp / mhp, (float)mp / mmp, hun / 5000.f, sta / 10000.f, (float)exp / expReq};
    string strings[] = {"HP: " + its(hp) + "/" + its(mhp), "Mana: " + its(mp) + "/" + its(mmp), "Satiety: " + its(hun / 50) + "%", "Stamina: " + its(sta / 100) + "%", "Exp: " + its(exp) + "/" + its(expReq)};
    static bool danger[] = {true, false, true, true, false};
    static short dangerInterval = 0;
    dangerInterval++;
    if (dangerInterval == 10) {
        dangerInterval = 0;
    }
    static float prevs[] = {percents[0], percents[1], percents[2], percents[3], percents[4]};
    static int prevTims[] = {0, 0, 0, 0, 0};
    for (int i = 0; i < 5; i++) {
        if (percents[i] != prevs[i]) {
            if (prevTims[i] == 0) {
                prevTims[i] = 100;
            } else if (prevTims[i] == 1) {
                prevs[i] = percents[i];
            }
        }
        if (prevTims[i] > 0) {
            prevTims[i]--;
        }
    }

    int k = 0;
    for (int i = 19; k < 5; i += 124) {
        if (prevTims[k]) {
            int wid1 = (int)(prevs[k] * 96);
            float flo = 1.;
            if (prevTims[k] < 10) {
                flo = prevTims[k] / 10.;
            }
            glColor4f(1, 1, 1, flo / 2);
            drawTileSuperSpe(i + 5, WIN1_HEIGHT - 21, Z_MENU, wid1, 14, menuTex, 106, 186 + k * 14, wid1, 14);
            color c = white; c.alpha = flo / 6;
            drawColorBox(i + 5, WIN1_HEIGHT - 21, Z_MENU + 1, i + 5 + wid1, WIN1_HEIGHT - 7, c);
        }
        int wid = (int)(percents[k] * 96);
        float flu = percents[k] / 2 + 0.5;
        glColor4f(flu, flu, flu, 1);
        drawTileSuperSpe(i + 5, WIN1_HEIGHT - 21, Z_MENU + 2, wid, 14, menuTex, 106, 186 + k * 14, wid, 14);
        if (danger[k] && percents[k] < .2) {
            color c = white; c.alpha = .5 - dangerInterval / 20.;
            drawColorBox(i + 5, WIN1_HEIGHT - 21, Z_MENU + 3, i + 5 + wid, WIN1_HEIGHT - 7, c);
        }
        glColor4f(1, 1, 1, 1);
        drawTileSuperSpe(i, WIN1_HEIGHT - 24, Z_MENU + 4, 106, 20, menuTex, 0, 192, 106, 20);
        renderText(strings[k], 4, i + 53, WIN1_HEIGHT - 20, Z_MENU + 5, CENTER, black);
        k++;
    }
}

void Start::drawMenuBox(int x1, int y1, int x2, int y2) {
    int cWid = x2 - x1 - 32;
    int cHei = y2 - y1 - 32;
    drawTileSuperSpe(x1     , y1     , Z_MENU, 16  , 16  , menuTex, 0 , 32, 16, 16);
    drawTileSuperSpe(x1 + 16, y1     , Z_MENU, cWid, 16  , menuTex, 16, 32, 16, 16);
    drawTileSuperSpe(x2 - 16, y1     , Z_MENU, 16  , 16  , menuTex, 32, 32, 16, 16);
    drawTileSuperSpe(x1     , y1 + 16, Z_MENU, 16  , cHei, menuTex, 0 , 48, 16, 16);
    drawTileSuperSpe(x1 + 16, y1 + 16, Z_MENU, cWid, cHei, menuTex, 16, 48, 16, 16);
    drawTileSuperSpe(x2 - 16, y1 + 16, Z_MENU, 16  , cHei, menuTex, 32, 48, 16, 16);
    drawTileSuperSpe(x1     , y2 - 16, Z_MENU, 16  , 16  , menuTex, 0 , 64, 16, 16);
    drawTileSuperSpe(x1 + 16, y2 - 16, Z_MENU, cWid, 16  , menuTex, 16, 64, 16, 16);
    drawTileSuperSpe(x2 - 16, y2 - 16, Z_MENU, 16  , 16  , menuTex, 32, 64, 16, 16);
}

const int shownItemStatsMin = 7;
const int shownItemStatsMax = 24;

color selectStatColor(int value, int i) {
    if (i == S_PENALTY) value = -value;
    color c;
    c.alpha = 0;
    if (value == 0) {
        return black;
    } else if (value > 0) {
        c.red = 0;
        c.green = 4 * min(value, 32);
        c.blue = 128 - 4 * min(value, 32);
    } else {
        c.red = 63 + 8 * min(-value, 24);
        c.green = 0;
        c.blue = 0;
    }
    return c;
}

void Start::renderMenu() {
    static const color weightColors[] = {tar, black, black, dark(dark(purple)), dark(purple), purple};
    static const color valueColors[] = {tar, black, dark(brown), dark(amber), olive, yellow};
    static const string dTypeNames[] = {"none", "bludgeon", "slashing", "piercing"};
    if (menuUp) {
        if (selected >= MAX_MENU_ITEMS + selectedShift) {
            selectedShift = selected - MAX_MENU_ITEMS + 1;
        } else if (selected < selectedShift) {
            selectedShift = selected;
        }

        ItemFolder* topFolder = folderStack.top();
        int numItems = topFolder->getNumItems();
        int numDisplayedItems = min(MAX_MENU_ITEMS, numItems);
        int height = 28 * numDisplayedItems + 88;
        int offset = (WIN1_HEIGHT - height) / 2;

        int place = offset + 72 + 28 * (selected - selectedShift);
        int dist = abs(curArrowY - place);
        int speed = 3 + dist / 20;
        if (curArrowY < place - speed) {
            curArrowY += speed;
        } else if (curArrowY > place + speed) {
            curArrowY -= speed;
        } else {
            curArrowY = place;
        }

        drawMenuBox(20, offset + 60, IM_WID + 20, offset + height - 16);

        Item* items = topFolder->getItems();
        for (int i = 0; i < numDisplayedItems; i++) {
            int k = i + selectedShift;
            ItemType* itemType = getItemType(items[k].itemType);
            graphic g = itemType->getGraphic();
            int d = offset + 64 + 28 * i;
            drawTile(28 + 8 * (k % 2), d, Z_MENU + i + 1, textures[g.tex], g.loc);
            renderText(capitalize(itemType->getName()), 2, 65 + 8 * (k % 2), d + 10, Z_MENU + i + 1, LEFT, black);
            if (items[k].quantityCharge > 1 && typeStacks[itemType->getType()]) {
                renderText(its(items[k].quantityCharge), 1, 50 + 8 * (k % 2), d + 16, Z_MENU + i + 2, LEFT, black);
            }
        }

        drawTileSpe(8, curArrowY, Z_MENU + 20, menuTex, arrowX[(interval % 40) / 4], arrowY[(interval % 40) / 4], 16);
        if (selectedShift > 0) {
            drawTileSuperSpe(IM_WID / 2 + 4, offset          + abs(interval % 24 / 2 - 6) + 36, Z_MENU, 24, 16, menuTex, 48, 48, 32, -16);
        }
        if (numItems - selectedShift > MAX_MENU_ITEMS) {
            drawTileSuperSpe(IM_WID / 2 + 4, offset + height - abs(interval % 24 / 2 - 6) - 10, Z_MENU, 24, 16, menuTex, 48, 32, 32,  16);
        }
        glColor4f(1, 1, 1, .5);
        drawTileSuperSpe(2, offset + 18, Z_MENU, IM_WID + 36, 16, menuTex, 0, 80, 64, 16);
        glColor4f(1, 1, 1, 1);
        renderText(menuActionNames[menuAction]    , 2, 4              , offset + 20, Z_MENU + 1, LEFT  , black);
        renderText(menuActionNames[menuAction + 1], 3, 20 + IM_WID / 2, offset + 20, Z_MENU + 1, CENTER, black);
        renderText(menuActionNames[menuAction + 2], 2, 36 + IM_WID    , offset + 20, Z_MENU + 1, RIGHT , black);

        Item selectedItem = items[selected];
        ItemType* selectedItemType = getItemType(selectedItem.itemType);
        int selectedItemTypeType = selectedItemType->getType();
        if (selectedItemTypeType >= 20) {
            static vector<pair<string, color> > lines;
            static unsigned int descLen;
            static Item prevItem;
            if (prevItem.itemType != selectedItem.itemType || prevItem.quantityCharge != selectedItem.quantityCharge || prevItem.form != selectedItem.form) {
                prevItem = items[selected];
                lines.clear();

                string desc = selectedItemType->getDescription() + " ";
                string nextLine = " ";
                int lastWord = 0;
                for(unsigned int i = 0; i < desc.size(); i++) {
                    if (desc[i] == ' ') {
                        if (nextLine.size() + (i - lastWord) > 29) {
                            lines.push_back(pair<string, color>(nextLine, dark(glaucous)));
                            nextLine = desc.substr(lastWord, i - lastWord);
                        } else {
                            nextLine += " " + desc.substr(lastWord, i - lastWord);
                        }
                        lastWord = i + 1;
                    }
                }
                lines.push_back(pair<string, color>(nextLine, dark(glaucous)));
                descLen = lines.size();
                lines.push_back(pair<string, color>("", black));

                int weightValue = selectedItemType->getStatValue(S_WEIGHT);
                lines.push_back(pair<string, color>(" Weight: " + its(weightValue) + " peb.", weightColors[numDigits0(weightValue)]));
                int valueValue = selectedItemType->getStatValue(S_VALUE);
                lines.push_back(pair<string, color>(" Value: " + its(valueValue) + " cp", valueColors[numDigits0(valueValue)]));

                color c;
                c.alpha = 0;
                if (selectedItemType->hasStat(S_IDAMAGE, false)) {
                    int damVal = selectedItemType->getStatValue(S_IDAMAGE);
                    c.red = 2 * min(damVal, 32);
                    c.blue = 2 * min(damVal, 32);
                    c.green = 2 * min(damVal, 32);
                    lines.push_back(pair<string, color>(" " + its(damVal) + " Damage (" + capitalize(dTypeNames[selectedItemType->getStatValue(S_DTYPE)]) + ")", c));
                }
                if (selectedItemType->hasStat(S_AC, false)) {
                    int acVal = selectedItemType->getStatValue(S_AC);
                    c.red = 2 * min(acVal, 32);
                    c.blue = 2 * min(acVal, 32);
                    c.green = 2 * min(acVal, 32);
                    lines.push_back(pair<string, color>(" " + its(acVal) + " AC", c));
                }

                for (int i = shownItemStatsMin; i <= shownItemStatsMax; i++) {
                    Stat* theStat = getStat(V_ITEM, i);
                    if (selectedItemType->hasStat(i, theStat->isItFloat())) {
                        if (theStat->isItFloat()) {
                            float value = selectedItemType->getStatValueF(i);
                            lines.push_back(pair<string, color>(" " + its0((int)value) + " " + capitalize(theStat->getName()), selectStatColor((int)value, i)));
                        } else {
                            int value = selectedItemType->getStatValue(i);
                            lines.push_back(pair<string, color>(" " + its0(value) + " " + capitalize(theStat->getName()), selectStatColor(value, i)));
                        }
                    }
                }
            }
            height = 32 + 30 + 12 * lines.size();
            offset = (WIN1_HEIGHT - height) / 2;
            drawMenuBox(IM_WID + 40, offset, IM_WID + 232, offset + height);
            glColor4f(1, 1, 1, .5);
            int nLen = selectedItemType->getName().size();
            drawTileSuperSpe(IM_WID + 132 - nLen * 7, offset + 9, Z_MENU + 1, 14 * nLen + 8, 32, menuTex, 0, 80, 64, 16);
            renderText(capitalize(selectedItemType->getName()), 5, IM_WID + 136, offset + 16, Z_MENU + 2, CENTER, black);
            int f = 4;
            for (unsigned int i = 0; i < lines.size(); i++) {
                if (i == descLen) f = 3;
                renderText(lines[i].first, f, IM_WID + 50, offset + 46 + 12 * i, Z_MENU + 1, LEFT, lines[i].second);
            }
        }
    }
}

void Start::renderMessages() {
    drawTileSuperSpe(0, WIN1_HEIGHT + 4, Z_MENU, 32, 96, menuTex, 0, 96, 32, 96);
    int x;
    for (x = 32; x < CWIN_WIDTH - 64; x += 32) {
        drawTileSuperSpe(x, WIN1_HEIGHT + 4, Z_MENU, 32, 96, menuTex, 32, 96, 32, 96);
    }
    drawTileSuperSpe(x, WIN1_HEIGHT + 4, Z_MENU, CWIN_WIDTH - x - 32, 96, menuTex, 32, 96, CWIN_WIDTH - x - 32, 96);
    drawTileSuperSpe(CWIN_WIDTH - 32, WIN1_HEIGHT + 4, Z_MENU, 32, 96, menuTex, 64, 96, 32, 96);

    for (int i = 7 - min((int)messages.size(), 7); i < 7; i++) {
        pair<string, color> completeMess = messages[messages.size() - 7 + i];
        renderText(completeMess.first, 2, 10, WIN1_HEIGHT + 15 + 11 * i, Z_MENU + 1, LEFT, completeMess.second);
    }

    int len = 1;
    int fl = 0;
    static unsigned int limit = WIN1_WIDTH / 8 - 2;
    for (map<int, pair<string, color> >::iterator i = statusies.begin(); i != statusies.end();) {
        string s = i->second.first;
        if (s.size() + len > limit) {
            fl++;
            len = 1;
        }
        color c = i->second.second;
        renderText(s, 3, len * 8, WIN1_HEIGHT - 40, Z_MENU, LEFT, c);
        len += s.size() + 2;
        if (++i != statusies.end()) {
            renderText(", ", 3, len * 8, WIN1_HEIGHT - 40 - fl * 14, Z_MENU, LEFT, black);
            len += 2;
        }
    }
}

void Start::renderGround() {

    Zone* z = player->getZone();
    int x = player->getUnit()->x;
    int y = player->getUnit()->y;

    glPushMatrix();
    glTranslatef(-(int)camX + WIN1_WIDTH / 2 - 16, -(int)camY + WIN1_HEIGHT / 2 - 16, 0);

    camX = x * TILE_SIZE;
    camY = y * TILE_SIZE;

    int maxXTiles = WIN1_WIDTH / TILE_SIZE;
    int maxYTiles = WIN1_HEIGHT / TILE_SIZE;
    int iMin = max(0, x - maxXTiles / 2 - 1);
    int jMin = max(0, y - maxYTiles / 2 - 2);
    int iMax = min(z->getWidth(), x + maxXTiles / 2 + 1);
    int jMax = min(z->getHeight(), y + maxYTiles / 2 + 2);

    updateAnims();

    for (int i = iMin; i < iMax; i++) {
        for (int j = jMin; j < jMax; j++) {
            float locX = (double)(i) * TILE_SIZE;
            float locY = (double)(j) * TILE_SIZE;
            bool isMemory = false;
            if (visibilities[i + j * z->getWidth()] < 2) {
                pair<int, int> botMems = player->getMemoryBottom(i, j);
                if (botMems.first == 5) {
                    drawColorBox(locX, locY, Z_EFFECT - 1, locX + TILE_SIZE, locY + TILE_SIZE, black);
                    glColor4f(1, 1, 1, 1);
                    continue;
                }
                isMemory = true;
            }
            Location* loc = z->getLocationAt(i, j);
            int h = loc->height;
            if (loc->isOpenDoor() || loc->isClosedDoor()) {
                h = MAX_HEIGHT;
            }
            float darkness = (double)(h + 8) / (MAX_HEIGHT + 8) / 2;

            if (isMemory) {
                darkness /= 2;
            }
            glColor4f(darkness, darkness, darkness, 1);
            Tile* tile = z->getTileAt(i, j);
            graphic g = tile->getGraphic();
            Texture* tex = getTexture(g.tex);
            int TZ;
            if (isMemory) {
                TZ = Z_EFFECT - 3;
            } else {
                TZ = Z_GROUND;
            }
            if (g.type == 0) {
                drawTile(locX, locY, TZ, tex, g.loc);
            } else if (g.type < 4) {
                int b = g.border;
                int wid = tex->Width / TILE_SIZE;
                int x1 = (g.loc % wid) * TILE_SIZE;
                int y1 = (g.loc / wid) * TILE_SIZE;

                Tile* tiles[8] = {z->safeGetTileAt(i-1, j-1), z->safeGetTileAt(i, j-1), z->safeGetTileAt(i+1, j-1),
                                z->safeGetTileAt(i-1, j), z->safeGetTileAt(i+1, j),
                                z->safeGetTileAt(i-1, j+1), z->safeGetTileAt(i, j+1), z->safeGetTileAt(i+1, j+1)};

                bool sm[8];
                for (int k = 0; k < 8; k++) {
                    sm[k] = tiles[k]->getGraphic().border == b && tiles[k] != tile;
                }

                if (g.type == 2 || g.type == 3) {
                    int heights[8] = {z->safeGetLocationAt(i-1, j-1)->height, z->safeGetLocationAt(i, j-1)->height, z->safeGetLocationAt(i+1, j-1)->height,
                                        z->safeGetLocationAt(i-1, j)->height, z->safeGetLocationAt(i+1, j)->height,
                                        z->safeGetLocationAt(i-1, j+1)->height, z->safeGetLocationAt(i, j+1)->height, z->safeGetLocationAt(i, j+1)->height};
                    for (int k = 0; k < 8; k++) {
                        if ((heights[k] >= h && g.type == 2) || (heights[k] <= h && g.type == 3)) {
                            sm[k] = false;
                        }
                    }
                }
                if (sm[3] && sm[1]) {
                    drawTileSpe(locX            , locY, TZ, tex, x1, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[3] && sm[1]) {
                    drawTileSpe(locX, locY, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (sm[3] && !sm[1]) {
                    drawTileSpe(locX, locY, TZ, tex, x1, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                } else if (!sm[3] && sm[0] && !sm[1]) {
                    drawTileSpe(locX, locY, TZ, tex, x1 + TILE_SIZE, y1, TILE_SIZE / 2);
                } else if (!sm[3] && !sm[0] && !sm[1]) {
                    drawTileSpe(locX, locY, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                }
                if (sm[1] && sm[4]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[1] && sm[4]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                } else if (sm[1] && !sm[4]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[1] && sm[2] && !sm[4]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1, TILE_SIZE / 2);
                } else if (!sm[1] && !sm[2] && !sm[4]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                }
                if (sm[4] && sm[6]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[4] && sm[6]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (sm[4] && !sm[6]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                } else if (!sm[4] && sm[7] && !sm[6]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE / 2, TILE_SIZE / 2);
                } else if (!sm[4] && !sm[7] && !sm[6]) {
                    drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                }
                if (sm[6] && sm[3]) {
                    drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[6] && sm[3]) {
                    drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                } else if (sm[6] && !sm[3]) {
                    drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[6] && sm[5] && !sm[3]) {
                    drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE / 2, TILE_SIZE / 2);
                } else if (!sm[6] && !sm[5] && !sm[3]) {
                    drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                }
            }
            if (isMemory) {
                glColor4f(.5, .5, .5, 1);
                pair<int, int> botMems = player->getMemoryBottom(i, j);
                if (botMems.first != 7 && botMems.first != 5) {
                    drawTile(locX, locY, Z_EFFECT - 2, getTexture(botMems.first), botMems.second);
                }
                pair<int, int> topMems = player->getMemoryTop(i, j);
                if (topMems.first != 7 && botMems.first != 5) {
                    drawTile(locX, locY, Z_EFFECT - 1, getTexture(topMems.first), topMems.second);
                }
            } else {
                glColor4f(1, 1, 1, 1);
                int struc = loc->structure;
                if (struc != S_NONE) {
                    drawTile(locX, locY, Z_STRUCT, structureTex, struc);
                }
                if (loc->hasItems()) {
                    Item* items = &(*loc->items)[0];
                    int numItems = loc->items->size();
                    for (int k = 0; k < numItems; k++) {
                        ItemType* itemType = getItemType(items[k].itemType);
                        drawTile(locX, locY, Z_ITEM + k, getTexture(itemType->getGraphic().tex), itemType->getGraphic().loc);
                    }
                }

                glColor4f(1, 1, 1, 0.5);
                if (splatters[i + j * z->getWidth()] < 255) {
                    drawTile(locX, locY, Z_SPLAT, splatterTex, splatters[i + j * z->getWidth()]);
                }
                glColor4f(1, 1, 1, 1);

                if (loc->hasUnit()) {
                    unitAnimTest(loc->unit, locX, locY);
                }
            }
        }
    }
    glColor4f(1, 1, 1, 1);
    drawBox(x + 50, y + 50, Z_UNIT, 4, interval, scarlet);
    updateEffects(x, y);
    renderAnims();
}

void Start::startRenderer() {
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIN1_WIDTH, WIN1_HEIGHT + CWIN_HEIGHT, 0, -10000, 10000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0., 0.1, 0., 0.);

	camX = 0;
	camY = 0;
	frameTime = 0;
}

//0-tiny, 1-small, 2-normal, 3-bold, 4-skinny, 5-large
int fontWid[] = {4, 6, 8 , 8, 6, 14};
int fontHei[] = {6, 9, 12, 12, 12, 20};
int offX[] = {  0, 132, 0, 128, 132, 266};
int offY[] = {192,   0, 0, 157, 72 ,   0};
int numX[] = {32, 16, 16, 16, 20, 16};

void Start::renderText(string text, int size, int x, int y, int z, int align, color c) {
    int a = 0;
    if (align == CENTER) {
        a = text.size() * fontWid[size] / 2;
    } else if (align == RIGHT) {
        a = text.size() * fontWid[size];
    }

    int w = fontWid[size];
    int h = fontHei[size];
    glColor3f(c.red / 255., c.green / 255., c.blue / 255.);
    for (unsigned int i = 0; i < text.size(); i++) {
        drawTileSuperSpe(x + i * w - a, y, z, w, h, fontTex, offX[size] + text[i] % numX[size] * w, offY[size] + text[i] / numX[size] * h, w, h);
    }

    glColor3f(1, 1, 1);
}
