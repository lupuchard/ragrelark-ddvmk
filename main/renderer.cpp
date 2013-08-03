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

#include "Start.h"
#include "graphics.h"

#define IM_WID 200
#define INTERVAL_MAX 360
#define MAX_MENU_ITEMS 12

#define SHOWN_ITEM_STATS_MIN 7
#define SHOWN_ITEM_STATS_MAX 24

static const String MENU_ACTION_NAMES[] = {"(t)hrow", "To ba(G)", "(d)rop", "E(Q)uip", "(e)at", "(t)hrow", "To ba(G)"};

static const char ARROW_X[] = {0, 16, 32, 48, 64,  0, 16, 32, 48, 64};
static const char ARROW_Y[] = {0,  0,  0,  0,  0, 16, 16, 16, 16, 16};

int interval = 0;

int curArrowY = WIN1_HEIGHT / 2;
int selectedShift = 0;

std::map<Status, std::pair<String, Color> > statusies;

void Start::addStatus(String name, Color c, Status type) {
    statusies[type] = std::pair<String, Color>(name, c);
}
void Start::removeStatus(Status type) {
    statusies.erase(type);
}

void tanBox() {
    TAN.gl();
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex2f(20, 20);
    glVertex2f(WIN1_WIDTH + SWIN_WIDTH - 20, 20);
    glVertex2f(WIN1_WIDTH + SWIN_WIDTH - 20, WIN1_HEIGHT + CWIN_HEIGHT - 20);
    glVertex2f(20, WIN1_HEIGHT + CWIN_HEIGHT - 20);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    WHITE.gl();
}

void Start::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    if (state == STATE_LOAD) {
        tanBox();
        ragd.renderText(loadStr, 3, 100, 100, Z_MENU, LEFT, BLACK);
    } else if (state == STATE_MAIN_MENU) {
        tanBox();
        startGameButton.render(&ragd);
        continueGameButton.render(&ragd);
    } else {
        renderGround();
        glPopMatrix();
        renderMenu();
        renderMessages();
        renderCircleSelect();
        renderBars();
        renderSidePanels();
    }

    glFlush();
    SDL_GL_SwapBuffers();

    interval++;
    if (interval == INTERVAL_MAX) {
        interval = 0;
    }
    frameTime++;
}

void Start::renderCircleSelect() {
    if (state == STATE_SPELL) {
        static const int siz = 4 + 3 * 28;
        static const int cx = CWIN_WIDTH - 4 - siz;
        static const int cy = WIN1_HEIGHT + 8;
        static const int locsX[] = {0, 0, 1, 1, 0, 0, 1, 0, 0, 1};
        static const int locsY[] = {0, 1, 3, 1, 2, 4, 2, 0, 3, 0};
        ragd.drawTileSpe(cx, cy, Z_MENU + 20, menuTex, 0, 212, siz);
        int k = 1;
        for (int j = 2; j >= 0; j--) {
            for (int i = 0; i < 3; i++) {
                int x = locsX[k] * 56;
                if (circleSelect[k]) x += 28;
                int y = locsY[k] * 28 + 308;
                ragd.drawTileSpe(cx + 1 + i * 29, cy + 1 + j * 29, Z_MENU + 30, menuTex, x, y, 28);
                k++;
            }
        }
    }
}

void Start::renderBars() {
    Unit* pUnit = player->getUnit();
    int hp  = pUnit->getStatValue(Stat::HP);
    int mhp = pUnit->getStatValue(Stat::MAXHP);
    int mp  = pUnit->getStatValue(Stat::MANA);
    int mmp = pUnit->getStatValue(Stat::MAXMANA);
    int exp = pUnit->getStatValue(Stat::EXP);
    int expReq = pUnit->getStatValue(Stat::EXPREQ);
    float hun = (float)pUnit->getStatValue(Stat::HUNGER);
    float sta = (float)pUnit->getStatValue(Stat::STAMINA);
    float percents[] = {(float)hp / mhp, (float)mp / mmp, hun / 5000.f, sta / 10000.f, (float)exp / expReq};
    String strings[] = {"HP: " + its(hp) + "/" + its(mhp), "Mana: " + its(mp) + "/" + its(mmp), "Satiety: " + its(hun / 50) + "%"
                            , "Stamina: " + its(sta / 100) + "%", "Exp: " + its(exp) + "/" + its(expReq)};
    static const bool danger[] = {true, false, true, true, false};
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
            ragd.drawTileSuperSpe(i + 5, WIN1_HEIGHT - 21, Z_MENU, wid1, 14, menuTex, 106, 186 + k * 14, wid1, 14);
            Color c = WHITE; c.alpha = flo / 6;
            ragd.drawColorBox(i + 5, WIN1_HEIGHT - 21, Z_MENU + 1, i + 5 + wid1, WIN1_HEIGHT - 7, c);
        }
        int wid = (int)(percents[k] * 96);
        float flu = percents[k] / 2 + 0.5;
        glColor4f(flu, flu, flu, 1);
        ragd.drawTileSuperSpe(i + 5, WIN1_HEIGHT - 21, Z_MENU + 2, wid, 14, menuTex, 106, 186 + k * 14, wid, 14);
        if (danger[k] && percents[k] < .2) {
            Color c = WHITE; c.alpha = .5 - dangerInterval / 20.;
            ragd.drawColorBox(i + 5, WIN1_HEIGHT - 21, Z_MENU + 3, i + 5 + wid, WIN1_HEIGHT - 7, c);
        }
        WHITE.gl();
        ragd.drawTileSuperSpe(i, WIN1_HEIGHT - 24, Z_MENU + 4, 106, 20, menuTex, 0, 192, 106, 20);
        ragd.renderText(strings[k], 4, i + 53, WIN1_HEIGHT - 20, Z_MENU + 5, CENTER, BLACK);
        k++;
    }
}

void Start::renderSidePanels() {
    int hei = SWIN_HEIGHT / 2 - 30;
    ragd.drawTileSuperSpe(WIN1_WIDTH, 30                  , Z_MENU, SWIN_WIDTH, hei, menuTex, 252, 192, SWIN_WIDTH, hei);
    ragd.drawTileSuperSpe(WIN1_WIDTH, 30 + SWIN_HEIGHT / 2, Z_MENU, SWIN_WIDTH, hei, menuTex, 252, 192, SWIN_WIDTH, hei);

    static const String panelNames[] = {"", "", "Stats", "Skills", "Inv", "", "", "Map", "Notes"};
    int k = 0;
    for (int i = PANEL_TOPSTART + 1; i < PANEL_TOPEND; i++) {
        int xd = 252;
        int siz = 2;
        if (i == topPanel) {
            xd = 317;
            siz = 3;
        }
        ragd.drawTileSuperSpe(WIN1_WIDTH + k * 65, 0, Z_MENU, 65, 30, menuTex, xd, 162, 65, 30);
        ragd.renderText(panelNames[i], siz, WIN1_WIDTH + k * 65 + 32, 12, Z_MENU + 1, CENTER, BLACK);
        k++;
    }
    k = 0;
    for (int i = PANEL_BOTTOMSTART + 1; i < PANEL_BOTTOMEND; i++) {
        int xd = 252;
        int siz = 2;
        if (i == botPanel) {
            xd = 317;
            siz = 3;
        }
        ragd.drawTileSuperSpe(WIN1_WIDTH + k * 65, SWIN_HEIGHT / 2, Z_MENU, 65, 30, menuTex, xd, 162, 65, 30);
        ragd.renderText(panelNames[i], siz, WIN1_WIDTH + k * 65 + 32, 12 + SWIN_HEIGHT / 2, Z_MENU + 1, CENTER, BLACK);
        k++;
    }

    static const int toff = 50;
    static const int loff = 20;
    if (topPanel == PANEL_STATS) {
        static const String statNames[] = {"Str: ", "Con: ", "Aff: ", "Int: ", "Per: ", "Dex: ", "Cha: "};
        Unit* p = player->getUnit();
        ragd.renderText("  HP: " + its(p->getStatValue(Stat::HP)) + "/" + its(p->getStatValue(Stat::MAXHP)), 2, loff + WIN1_WIDTH, toff, Z_MENU + 1, LEFT, FOREST);
        ragd.renderText("  MANA: " + its(p->getStatValue(Stat::MANA)) + "/" + its(p->getStatValue(Stat::MAXMANA)), 2, loff + WIN1_WIDTH, toff + 20, Z_MENU + 1, LEFT, NAVY);
        for (int i = Stat::STR; i <= Stat::CHA; i++) {
            int main = p->getStatValue(i);
            String s = statNames[i - Stat::STR] + "\\z" + its(main);
            ragd.renderText(s, 2, loff + WIN1_WIDTH, toff + (i - Stat::STR + 2) * 20, Z_MENU + 1, LEFT, BLACK);
        }
        ragd.renderText("Defense: \\q" + its(p->getStatValue(Stat::DEFENSE)), 2, loff + WIN1_WIDTH, toff + 200, Z_MENU + 1, LEFT, BLACK);
        ragd.renderText("Exp pool: \\q" + its(player->getXpBank()), 2, loff + WIN1_WIDTH, toff + 240, Z_MENU + 1, LEFT, TEAL.darken());

        ragd.renderText("Level " + its(player->getUnit()->getStatValue(Stat::LEVEL)) + "   ", 2, WIN1_WIDTH + SWIN_WIDTH, toff + 40, Z_MENU + 1, RIGHT, BLACK);
        ragd.renderText("\\q" + its(player->getUnit()->theTime) + "\\z ticks", 2, WIN1_WIDTH + SWIN_WIDTH, toff + 80, Z_MENU + 1, RIGHT, RED.darken());
    } else if (topPanel == PANEL_SKILLS) {
        for (int i = 0; i < Stat::getNumSkills(); i++) {
            Skill* skill = Stat::getSkill(i);
            int x = skill->displayLoc.x + WIN1_WIDTH;
            int y = skill->displayLoc.y + 30;
            ragd.drawTileSuperSpe(x - 3, y - 3, Z_MENU + 1, TILE_SIZE + 6, TILE_SIZE + 6, menuTex, 213, 161, 38, 38);
            if (!skill->active) GRAY.gl();
            ragd.drawTileSpe(x, y, Z_MENU + 2, menuTex, 216, 164, TILE_SIZE);
            WHITE.gl();
            ragd.drawTile(x, y, Z_MENU + 3, skill->graphic.tex, skill->graphic.loc);
            int lev = player->getSkillLevel(skill);
            int prog = player->getSkillExpPercent(skill);
            if (lev || prog) {
                ragd.renderText(its(lev), 1, x + 1, y, Z_MENU + 4, LEFT, WHITE);
                if (prog) {
                    String progs;
                    if (numDigits0(prog) == 1) progs = "0" + its(prog);
                    else progs = its(prog);
                    ragd.renderText(progs, 1, x + 8, y, Z_MENU + 5, LEFT, SILVER);
                    ragd.renderText(".", 1, x + 4, y, Z_MENU + 5, LEFT, SILVER);
                }
            }
        }
    }

    if (botPanel == PANEL_MINIMAP) {
        //MINIMAP TIME
        Zone* z = player->getZone();
        int len = z->getWidth() * z->getHeight() * 3;
        unsigned char datas[len];
        int k = 0;
        for (int j = z->getHeight() - 1; j >= 0; j--) {
            for (int i = 0; i < z->getWidth(); i++) {
                Coord pos = Coord(i, j);
                Location* loc = z->getLocationAt(pos);
                Color c;
                if (player->getUnit()->pos == pos) {
                    c = MAGENTA;
                } else if (visibilities[j * z->getWidth() + i] < 2) {
                    std::pair<int, int> p = player->getMemoryBottom(pos);
                    if (p.first == 5) {
                        c = BLACK;
                    } else {
                        if (loc->structure == S_STAIRUP || loc->structure == S_STAIRDOWN) {
                            c = SILVER;
                        } else if (loc->isClosedDoor() || loc->isOpenDoor()) {
                            c = BROWN;
                        } else if (loc->structure == S_HIDDENDOOR) {
                            c = Color(MAX_HEIGHT * 2);
                        } else {
                            c = Color(loc->height * 2);
                        }
                    }
                } else if (loc->hasUnit()) {
                    c = RED;
                } else if (loc->structure != S_NONE) {
                    if (loc->structure == S_STAIRUP || loc->structure == S_STAIRDOWN) {
                        c = SILVER;
                    } else if (loc->isOpenDoor()) {
                        c = TAN.darken();
                    } else if (loc->isClosedDoor()) {
                        c = BROWN;
                    } else if (loc->structure == S_ROCK) {
                        c = CHARCOAL;
                    } else if (loc->structure == S_HIDDENDOOR) {
                        c = Color(MAX_HEIGHT * 3 + 32);
                    } else {
                        c = PINK;
                    }
                } else if (loc->hasItems()) {
                    c = GREEN;
                } else {
                    c = Color(loc->height * 3 + 32);
                }
                datas[k++] = c.red;
                datas[k++] = c.green;
                datas[k++] = c.blue;
            }
        }
        for (; k < len; k++) {
            datas[k] = 0;
        }
        glRasterPos3i(WIN1_WIDTH + 30, SWIN_HEIGHT / 2 + 60 + z->getHeight() * 3, Z_MENU + 1);
        glPixelZoom(3, 3);
        glDrawPixels(z->getWidth(), z->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, datas);

        ragd.renderText(player->getZone()->getName(), 2, loff + WIN1_WIDTH, SWIN_HEIGHT / 2 + toff + 240, Z_MENU + 1, LEFT, CHARCOAL);
    } else if (botPanel == PANEL_NOTES) {
        for (int i = 0; i < NUM_NOTELINES; i++) {
            ragd.renderText(theNotes[i], 2, WIN1_WIDTH + loff, i * 12 + SWIN_HEIGHT / 2 + 30, Z_MENU + 1, LEFT, BLACK);
        }
        if (notesSelected && notesSelected < NUM_NOTELINES) {
            if (interval % 32 < 16) {
                drawBox(WIN1_WIDTH + 8 * theNotes[notesSelected].size() + loff - 4, notesSelected * 12 + SWIN_HEIGHT / 2 + 28, Z_MENU + 2, 1, 0, BLACK);
            }
        }
    }
}

void Start::drawMenuBox(int x1, int y1, int x2, int y2) {
    int cWid = x2 - x1 - 32;
    int cHei = y2 - y1 - 32;
    ragd.drawTileSuperSpe(x1     , y1     , Z_MENU, 16  , 16  , menuTex, 0 , 32, 16, 16);
    ragd.drawTileSuperSpe(x1 + 16, y1     , Z_MENU, cWid, 16  , menuTex, 16, 32, 16, 16);
    ragd.drawTileSuperSpe(x2 - 16, y1     , Z_MENU, 16  , 16  , menuTex, 32, 32, 16, 16);
    ragd.drawTileSuperSpe(x1     , y1 + 16, Z_MENU, 16  , cHei, menuTex, 0 , 48, 16, 16);
    ragd.drawTileSuperSpe(x1 + 16, y1 + 16, Z_MENU, cWid, cHei, menuTex, 16, 48, 16, 16);
    ragd.drawTileSuperSpe(x2 - 16, y1 + 16, Z_MENU, 16  , cHei, menuTex, 32, 48, 16, 16);
    ragd.drawTileSuperSpe(x1     , y2 - 16, Z_MENU, 16  , 16  , menuTex, 0 , 64, 16, 16);
    ragd.drawTileSuperSpe(x1 + 16, y2 - 16, Z_MENU, cWid, 16  , menuTex, 16, 64, 16, 16);
    ragd.drawTileSuperSpe(x2 - 16, y2 - 16, Z_MENU, 16  , 16  , menuTex, 32, 64, 16, 16);
}

Color selectStatColor(int value) {
    if (value == 0) {
        return BLACK;
    } else if (value > 0) {
        return Color(0, 4 * std::min(value, 32), 128 - 4 * std::min(value, 32));
    }
    return Color(63 + 8 * std::min(-value, 24), 0, 0);
}

void Start::renderMenu() {
    static const Color weightColors[] = {TAR, BLACK, BLACK, PURPLE.darken().darken(), PURPLE.darken(), PURPLE};
    static const Color valueColors[] = {TAR, BLACK, BROWN.darken(), AMBER.darken(), OLIVE, YELLOW};
    if (state == STATE_MENU) {
        if (selected >= MAX_MENU_ITEMS + selectedShift) {
            selectedShift = selected - MAX_MENU_ITEMS + 1;
        } else if (selected < selectedShift) {
            selectedShift = selected;
        }

        ItemFolder* topFolder = folderStack.top();
        int numItems = topFolder->getNumItems();
        int numDisplayedItems = std::min(std::min(MAX_MENU_ITEMS, numItems), numItems - selectedShift);
        int height = 28 * numDisplayedItems + 88;
        int offset = (WIN1_HEIGHT - height) / 2;

        int place = offset + 72 + 28 * (selected - selectedShift);
        int dist = std::abs(curArrowY - place);
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
            ItemType* itemType = items[k].getType();
            Graphic g = itemType->getGraphic(items[k].quantityCharge);
            int d = offset + 64 + 28 * i;
            ragd.drawTile(28 + 8 * (k % 2), d, Z_MENU + i + 1, g.tex, g.loc);
            String name;
            if (items[k].quantityCharge > 1 && itemType->getStack()) {
                name = pluralize(capitalize(itemType->getName()));
                ragd.renderText(its(items[k].quantityCharge), 1, 50 + 8 * (k % 2), d + 16, Z_MENU + i + 2, LEFT, BLACK);
            } else {
                name = capitalize(itemType->getName());
            }
            ragd.renderText(name, 2, 65 + 8 * (k % 2), d + 10, Z_MENU + i + 1, LEFT, BLACK);
        }

        ragd.drawTileSpe(8, curArrowY, Z_MENU + 20, menuTex, ARROW_X[(interval % 40) / 4], ARROW_Y[(interval % 40) / 4], 16);
        if (selectedShift > 0) {
            ragd.drawTileSuperSpe(IM_WID / 2 + 4, offset          + std::abs(interval % 24 / 2 - 6) + 36, Z_MENU, 24, 16, menuTex, 48, 48, 32, -16);
        }
        if (numItems - selectedShift > MAX_MENU_ITEMS) {
            ragd.drawTileSuperSpe(IM_WID / 2 + 4, offset + height - std::abs(interval % 24 / 2 - 6) - 10, Z_MENU, 24, 16, menuTex, 48, 32, 32,  16);
        }
        glColor4f(1, 1, 1, .5);
        ragd.drawTileSuperSpe(2, offset + 18, Z_MENU, IM_WID + 36, 16, menuTex, 0, 80, 64, 16);
        WHITE.gl();
        ragd.renderText(MENU_ACTION_NAMES[menuAction]    , 2, 4              , offset + 20, Z_MENU + 1, LEFT  , BLACK);
        ragd.renderText(MENU_ACTION_NAMES[menuAction + 1], 3, 20 + IM_WID / 2, offset + 20, Z_MENU + 1, CENTER, BLACK);
        ragd.renderText(MENU_ACTION_NAMES[menuAction + 2], 2, 36 + IM_WID    , offset + 20, Z_MENU + 1, RIGHT , BLACK);

        Item selectedItem = items[selected];
        ItemType* selectedItemType = selectedItem.getType();
        if (!selectedItemType->isSlot() && !selectedItemType->isFolder()) {
            static std::vector<std::pair<String, Color> > lines;
            static unsigned int descLen;
            static Item prevItem;
            if (prevItem.itemType != selectedItem.itemType || prevItem.quantityCharge != selectedItem.quantityCharge || prevItem.form != selectedItem.form) {
                prevItem = items[selected];
                lines.clear();

                String desc = selectedItemType->getDescription() + " ";
                String nextLine = " ";
                int lastWord = 0;
                for(unsigned int i = 0; i < desc.size(); i++) {
                    if (desc[i] == ' ') {
                        if (nextLine.size() + (i - lastWord) > 29) {
                            lines.push_back(std::make_pair(nextLine, GLAUCOUS.darken()));
                            nextLine = desc.substr(lastWord, i - lastWord);
                        } else {
                            nextLine += " " + desc.substr(lastWord, i - lastWord);
                        }
                        lastWord = i + 1;
                    }
                }
                lines.push_back(std::make_pair(nextLine, GLAUCOUS.darken()));
                descLen = lines.size();
                lines.push_back(std::make_pair("", BLACK));

                int weightValue = selectedItemType->getStatValue(Stat::WEIGHT);
                lines.push_back(std::make_pair(" Weight: " + its(weightValue) + " pebbles", weightColors[numDigits0(weightValue)]));
                int valueValue = selectedItemType->getStatValue(Stat::VALUE);
                lines.push_back(std::make_pair(" Value: " + its(valueValue) + " cp", valueColors[numDigits0(valueValue)]));

                for (int i = 0; i < Stat::getNum(V_ITEM); i++) {
                    Stat* stat = Stat::get(V_ITEM, i);
                    if (stat->getDisplay()) {
                        int val = selectedItemType->getStatValue(i);
                        if (val) {
                            switch(stat->getDisplay()) {
                                case DISP_NORM:
                                    lines.push_back(std::make_pair(" " + stat->getDisplayName() + ": " + its(val), BLACK));
                                    break;
                                case DISP_PRE:
                                    lines.push_back(std::make_pair(" " + its(val) + " " + stat->getDisplayName(), BLACK));
                                    break;
                                case DISP_PLUS: {
                                    String s = its(val);
                                    if (val > 0) s = "+" + s;
                                    lines.push_back(std::make_pair(" " + stat->getDisplayName() + ": " + s, selectStatColor(val)));
                                    } break;
                                case DISP_GOOD:
                                    lines.push_back(std::make_pair(" " + stat->getDisplayName() + ": " + its(val), selectStatColor(val)));
                                    break;
                                case DISP_BAD:
                                    lines.push_back(std::make_pair(" " + stat->getDisplayName() + ": " + its(val), selectStatColor(-val)));
                                    break;
                                default: break;
                            }
                        }
                    }
                }

                static const String DTYPE_NAMES[] = {"", "Bludgeoning", "Slashing", "Piercing", "Sinister"};
                WeapType* weapType = selectedItemType->getType()->weapType;
                if (weapType) lines.push_back(std::make_pair("  " + DTYPE_NAMES[weapType->damageType], BLACK));

                if (selectedItemType->getType()->edible) lines.push_back(std::make_pair("  Edible", BLACK));

                lines.push_back(std::make_pair("", BLACK));
                //then it displays any abilities it may have
                for (std::set<unsigned short>::iterator i = selectedItemType->getAbilitiesBegin(); i != selectedItemType->getAbilitiesEnd(); ++i) {
                    lines.push_back(std::make_pair("  " + capitalize(Ability::get(*i)->getName()), TEAL.darken()));
                }
            }
            height = 32 + 30 + 12 * lines.size();
            offset = (WIN1_HEIGHT - height) / 2;
            drawMenuBox(IM_WID + 40, offset, IM_WID + 232, offset + height);
            glColor4f(1, 1, 1, .5);
            int nLen = selectedItemType->getName().size();
            ragd.drawTileSuperSpe(IM_WID + 132 - nLen * 7, offset + 9, Z_MENU + 1, 14 * nLen + 8, 32, menuTex, 0, 80, 64, 16);
            ragd.renderText(capitalize(selectedItemType->getName()), 5, IM_WID + 136, offset + 16, Z_MENU + 2, CENTER, BLACK);
            int f = 4;
            for (unsigned int i = 0; i < lines.size(); i++) {
                if (i == descLen) f = 3;
                ragd.renderText(lines[i].first, f, IM_WID + 50, offset + 46 + 12 * i, Z_MENU + 1, LEFT, lines[i].second);
            }
        }
    }
}

void Start::renderMessages() {
    using namespace std;
    ragd.drawTileSuperSpe(0, WIN1_HEIGHT + 4, Z_MENU, 32, 96, menuTex, 0, 96, 32, 96);
    int x;
    for (x = 32; x < CWIN_WIDTH - 64; x += 32) {
        ragd.drawTileSuperSpe(x, WIN1_HEIGHT + 4, Z_MENU, 32, 96, menuTex, 32, 96, 32, 96);
    }
    ragd.drawTileSuperSpe(x, WIN1_HEIGHT + 4, Z_MENU, CWIN_WIDTH - x - 32, 96, menuTex, 32, 96, CWIN_WIDTH - x - 32, 96);
    ragd.drawTileSuperSpe(CWIN_WIDTH - 32, WIN1_HEIGHT + 4, Z_MENU, 32, 96, menuTex, 64, 96, 32, 96);

    for (int i = 7 - min((int)messages.size(), 7); i < 7; i++) {
        pair<string, Color> completeMess = messages[messages.size() - 7 + i];
        ragd.renderText(completeMess.first, 2, 10, WIN1_HEIGHT + 15 + 11 * i, Z_MENU + 1, LEFT, completeMess.second);
    }

    int len = 1;
    int fl = 0;
    static unsigned int limit = WIN1_WIDTH / 8 - 2;
    for (map<Status, pair<string, Color> >::iterator i = statusies.begin(); i != statusies.end();) {
        string s = i->second.first;
        if (s.size() + len > limit) {
            fl++;
            len = 1;
        }
        Color c = i->second.second;
        ragd.renderText(s, 3, len * 8, WIN1_HEIGHT - 40, Z_MENU, LEFT, c);
        len += s.size() + 2;
        if (++i != statusies.end()) {
            ragd.renderText(", ", 3, len * 8, WIN1_HEIGHT - 40 - fl * 14, Z_MENU, LEFT, BLACK);
            len += 2;
        }
    }
}

typedef struct {
    int loc;
    int tex;
    float darkness;
} Rendaten;

bool is_to_be_deleted(Unit* u) {
    return !(u->graphic.border);
}

void Start::renderGround() {
    Zone* z = player->getZone();
    int x = player->getUnit()->pos.x;
    int y = player->getUnit()->pos.y;

    glPushMatrix();
    glTranslatef(-(int)ragd.camX + WIN1_WIDTH / 2 - 16, -(int)ragd.camY + WIN1_HEIGHT / 2 - 16, 0);

    ragd.camX = x * TILE_SIZE;
    ragd.camY = y * TILE_SIZE;

    using namespace std;
    int maxXTiles = WIN1_WIDTH / TILE_SIZE;
    int maxYTiles = WIN1_HEIGHT / TILE_SIZE;
    int iMin = max(0, x - maxXTiles / 2 - 1);
    int jMin = max(0, y - maxYTiles / 2 - 2);
    int iMax = min(z->getWidth(), x + maxXTiles / 2 + 1);
    int jMax = min(z->getHeight(), y + maxYTiles / 2 + 2);

    raga.updateAnims();
    unitDeleteList.erase(remove_if(unitDeleteList.begin(), unitDeleteList.end(), is_to_be_deleted), unitDeleteList.end());

    Rendaten renderAtEnd[(iMax - iMin) * (jMax - jMin)];
    int tot = 0;
    for (int i = iMin; i < iMax; i++) {
        for (int j = jMin; j < jMax; j++) {
            renderAtEnd[tot] = {-1, -1, -1};
            tot++;
            Coord pos = Coord(i, j);
            int locX = i * TILE_SIZE;
            int locY = j * TILE_SIZE;
            bool isMemory = false;
            if (visibilities[i + j * z->getWidth()] < 2) {
                pair<int, int> botMems = player->getMemoryBottom(pos);
                if (botMems.first == 5) {
                    ragd.drawColorBox(locX, locY, Z_EFFECT - 1, locX + TILE_SIZE, locY + TILE_SIZE, BLACK);
                    WHITE.gl();
                    continue;
                }
                isMemory = true;
            }
            Location* loc = z->getLocationAt(pos);
            int h = loc->height;
            if (loc->isOpenDoor() || loc->isClosedDoor() || loc->structure == S_HIDDENDOOR) {
                h = MAX_HEIGHT;
            }
            float darkness = (double)(h * 2 + 16) / (MAX_HEIGHT*3);

            if (isMemory) {
                darkness /= 2;
            }
            glColor4f(darkness, darkness, darkness, 1);
            Tile* tile = Tile::get(loc->tile);
            Graphic g = tile->getGraphic();
            if (tile->getOver()) {
                renderAtEnd[tot - 1] = {g.loc, g.tex->getIndex(), darkness};
                tile = tile->getOver();
                g = tile->getGraphic();
            }
            int TZ;
            if (isMemory) {
                TZ = Z_EFFECT - 3;
            } else {
                TZ = Z_GROUND;
            }
            if (g.type == TT_NORMAL) {
                ragd.drawTile(locX, locY, TZ, g.tex, g.loc);
            } else if (g.type == TT_SMOOTH || g.type == TT_SMOOTHDOWN || g.type == TT_SMOOTHUP) {
                int b = g.border;
                int wid = g.tex->getWidth() / TILE_SIZE;
                int x1 = (g.loc % wid) * TILE_SIZE;
                int y1 = (g.loc / wid) * TILE_SIZE;

                Location* locs[8] = {z->safeGetLocationAt(pos + DIRS[7]), z->safeGetLocationAt(pos + DIRS[8]), z->safeGetLocationAt(pos + DIRS[9]),
                                     z->safeGetLocationAt(pos + DIRS[4]), z->safeGetLocationAt(pos + DIRS[6]),
                                     z->safeGetLocationAt(pos + DIRS[1]), z->safeGetLocationAt(pos + DIRS[2]), z->safeGetLocationAt(pos + DIRS[3])};
                Tile* tiles[8];
                for (int k = 0; k < 8; k++) tiles[k] = Tile::get(locs[k]->tile);

                bool sm[8];
                for (int k = 0; k < 8; k++) {
                    sm[k] = (tiles[k]->getGraphic().border == b) && (tiles[k] != tile);
                }

                if (g.type == TT_SMOOTHUP || g.type == TT_SMOOTHDOWN) {
                    int heights[8];
                    for (int k = 0; k < 8; k++) heights[k] = locs[k]->height;

                    for (int k = 0; k < 8; k++) {
                        if ((heights[k] >= h - 2 && g.type == 2) || (heights[k] <= h + 2 && g.type == 3)) {
                            sm[k] = false;
                        }
                    }
                }
                if (sm[3] && sm[1]) {
                    ragd.drawTileSpe(locX            , locY, TZ, g.tex, x1, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[3] && sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, g.tex, x1 + TILE_SIZE, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (sm[3] && !sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, g.tex, x1, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                } else if (!sm[3] && sm[0] && !sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, g.tex, x1 + TILE_SIZE, y1, TILE_SIZE / 2);
                } else if (!sm[3] && !sm[0] && !sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, g.tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                }
                if (sm[1] && sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, g.tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[1] && sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, g.tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                } else if (sm[1] && !sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, g.tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[1] && sm[2] && !sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, g.tex, x1 + TILE_SIZE * 3 / 2, y1, TILE_SIZE / 2);
                } else if (!sm[1] && !sm[2] && !sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, g.tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                }
                if (sm[4] && sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[4] && sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (sm[4] && !sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                } else if (!sm[4] && sm[7] && !sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE / 2, TILE_SIZE / 2);
                } else if (!sm[4] && !sm[7] && !sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                }
                if (sm[6] && sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, g.tex, x1, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[6] && sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, g.tex, x1, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                } else if (sm[6] && !sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[6] && sm[5] && !sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE, y1 + TILE_SIZE / 2, TILE_SIZE / 2);
                } else if (!sm[6] && !sm[5] && !sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, g.tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                }
            }
            if (isMemory) {
                glColor4f(.5, .5, .5, 1);
                pair<int, int> botMems = player->getMemoryBottom(pos);
                if (botMems.first != 7 && botMems.first != 5) {
                    ragd.drawTile(locX, locY, Z_EFFECT - 2, Texture::get(botMems.first), botMems.second);
                }
                pair<int, int> topMems = player->getMemoryTop(pos);
                if (topMems.first != 7 && botMems.first != 5) {
                    ragd.drawTile(locX, locY, Z_EFFECT - 1, Texture::get(topMems.first), topMems.second);
                }
            } else {
                glColor4f(1, 1, 1, 1);
                int struc = loc->structure;
                if (struc != S_NONE) {
                    ragd.drawTile(locX, locY, Z_STRUCT, structureTex, struc);
                }

                glColor4f(1, 1, 1, 0.5);
                if (loc->debris2) {
                    ragd.drawTile(locX, locY, Z_SPLAT, splatterTex, loc->debris2);
                }
                if (loc->debris1) {
                    ragd.drawTile(locX, locY, Z_SPLAT + 1, splatterTex, loc->debris1);
                }
                WHITE.gl();
                if (loc->hasItems()) {
                    Item* items = &(*loc->items)[0];
                    int numItems = loc->items->size();
                    for (int k = 0; k < numItems; k++) {
                        ItemType* itemType = items[k].getType();
                        Graphic g = itemType->getGraphic(items[k].quantityCharge);
                        ragd.drawTile(locX, locY, Z_ITEM + k, g.tex, g.loc);
                    }
                }
                if (loc->hasUnit()) {
                    raga.unitAnimTest(loc->unit, locX, locY);
                }
            }
        }
    }
    WHITE.gl();

    raga.renderAnims();

    tot = 0;
    for (int i = iMin; i < iMax; i++) {
        for (int j = jMin; j < jMax; j++) {
            if (renderAtEnd[tot].loc != -1) {
                int locX = i * TILE_SIZE;
                int locY = j * TILE_SIZE;
                float darkness = renderAtEnd[tot].darkness;
                glColor4f(darkness, darkness, darkness, 1);
                ragd.drawTile(locX, locY, Z_FOREGROUND, Texture::get(renderAtEnd[tot].tex), renderAtEnd[tot].loc);
            }
            tot++;
        }
    }
    WHITE.gl();

    updateEffects(x, y);
    if (state == STATE_TARGET) {
        //51-55
        //67-71
        int num = (interval % 40) / 4;
        if (num >= 5) num += 62;
        else num += 51;
        ragd.drawTile(unitsInRange[stIndex]->pos.x * TILE_SIZE, unitsInRange[stIndex]->pos.y * TILE_SIZE, Z_MENU, menuTex, num);
    }
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
	glOrtho(0, WIN1_WIDTH + SWIN_WIDTH, WIN1_HEIGHT + CWIN_HEIGHT, 0, -10000, 10000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0., 0.1, 0., 0.);

	frameTime = 0;

	structureTex    = Texture::get("structures.png");
    menuTex         = Texture::get("menu.png");
    fontTex         = Texture::get("font.png");
    splatterTex     = Texture::get("splatters.png");
    attackAnimsTex  = Texture::get("attackAnims.png");
    playerTex       = Texture::get("player.png");

    startGameButton.setGraphics(Texture::get("buttons.png"), ORIGIN, 280, 50);
    startGameButton.setText(5, "New Game", BLACK);
    startGameButton.setPos(Coord((WIN1_WIDTH + SWIN_WIDTH) / 2, (WIN1_HEIGHT + CWIN_HEIGHT) / 2), Z_MENU);
    startGameButton.setFunc(boost::bind(&Start::startGame, this));

    continueGameButton.setGraphics(Texture::get("buttons.png"), ORIGIN, 280, 50);
    continueGameButton.setText(5, "Continue Game", BLACK);
    continueGameButton.setPos(Coord((WIN1_WIDTH + SWIN_WIDTH) / 2, (WIN1_HEIGHT + CWIN_HEIGHT) / 2 + 60), Z_MENU);
    continueGameButton.setFunc(boost::bind(&Start::continueGame, this));

    ragd = RagDrawer(TILE_SIZE, menuTex, fontTex);
	raga = RagAnim(&ragd, attackAnimsTex);

	createEffect(P_ARROW, 0, 0);
}
