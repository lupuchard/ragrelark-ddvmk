#include "Start.h"
#include "graphics.h"

#define IM_WID 200
#define INTERVAL_MAX 360
#define MAX_MENU_ITEMS 12

#define SHOWN_ITEM_STATS_MIN 7
#define SHOWN_ITEM_STATS_MAX 24

const string menuActionNames[] = {"(R)ead", "E(x)amine", "To ba(G)", "(d)rop", "E(Q)uip", "(e)at", "(R)ead", "E(x)amine"};

const char arrowX[] = {0, 16, 32, 48, 64,  0, 16, 32, 48, 64};
const char arrowY[] = {0,  0,  0,  0,  0, 16, 16, 16, 16, 16};

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

void Start::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    renderGround();

    glPopMatrix();

    renderMenu();
    renderMessages();
    renderCircleSelect();
    renderBars();
    renderSidePanels();

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
        ragd.drawTileSpe(cx, cy, Z_MENU + 20, getMenuTex(), 0, 212, siz);
        int k = 1;
        for (int j = 2; j >= 0; j--) {
            for (int i = 0; i < 3; i++) {
                int x = locsX[k] * 56;
                if (circleSelect[k]) x += 28;
                int y = locsY[k] * 28 + 308;
                ragd.drawTileSpe(cx + 1 + i * 29, cy + 1 + j * 29, Z_MENU + 30, getMenuTex(), x, y, 28);
                k++;
            }
        }
    }
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
            ragd.drawTileSuperSpe(i + 5, WIN1_HEIGHT - 21, Z_MENU, wid1, 14, getMenuTex(), 106, 186 + k * 14, wid1, 14);
            color c = white; c.alpha = flo / 6;
            ragd.drawColorBox(i + 5, WIN1_HEIGHT - 21, Z_MENU + 1, i + 5 + wid1, WIN1_HEIGHT - 7, c);
        }
        int wid = (int)(percents[k] * 96);
        float flu = percents[k] / 2 + 0.5;
        glColor4f(flu, flu, flu, 1);
        ragd.drawTileSuperSpe(i + 5, WIN1_HEIGHT - 21, Z_MENU + 2, wid, 14, getMenuTex(), 106, 186 + k * 14, wid, 14);
        if (danger[k] && percents[k] < .2) {
            color c = white; c.alpha = .5 - dangerInterval / 20.;
            ragd.drawColorBox(i + 5, WIN1_HEIGHT - 21, Z_MENU + 3, i + 5 + wid, WIN1_HEIGHT - 7, c);
        }
        glColor4f(1, 1, 1, 1);
        ragd.drawTileSuperSpe(i, WIN1_HEIGHT - 24, Z_MENU + 4, 106, 20, getMenuTex(), 0, 192, 106, 20);
        renderText(strings[k], 4, i + 53, WIN1_HEIGHT - 20, Z_MENU + 5, CENTER, black);
        k++;
    }
}

void Start::renderSidePanels() {
    int hei = SWIN_HEIGHT / 2 - 30;
    ragd.drawTileSuperSpe(WIN1_WIDTH, 30                  , Z_MENU, SWIN_WIDTH, hei, getMenuTex(), 252, 192, SWIN_WIDTH, hei);
    ragd.drawTileSuperSpe(WIN1_WIDTH, 30 + SWIN_HEIGHT / 2, Z_MENU, SWIN_WIDTH, hei, getMenuTex(), 252, 192, SWIN_WIDTH, hei);

    static const string panelNames[] = {"", "", "Stats", "Skills", "Inv", "", "", "Map", "Notes"};
    int k = 0;
    for (int i = PANEL_TOPSTART + 1; i < PANEL_TOPEND; i++) {
        int xd = 252;
        int siz = 2;
        if (i == topPanel) {
            xd = 317;
            siz = 3;
        }
        ragd.drawTileSuperSpe(WIN1_WIDTH + k * 65, 0, Z_MENU, 65, 30, getMenuTex(), xd, 162, 65, 30);
        renderText(panelNames[i], siz, WIN1_WIDTH + k * 65 + 32, 12, Z_MENU + 1, CENTER, black);
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
        ragd.drawTileSuperSpe(WIN1_WIDTH + k * 65, SWIN_HEIGHT / 2, Z_MENU, 65, 30, getMenuTex(), xd, 162, 65, 30);
        renderText(panelNames[i], siz, WIN1_WIDTH + k * 65 + 32, 12 + SWIN_HEIGHT / 2, Z_MENU + 1, CENTER, black);
        k++;
    }

    static const int toff = 50;
    static const int loff = 20;
    if (topPanel == PANEL_STATS) {
        static const string statNames[] = {"Str: ", "Con: ", "Aff: ", "Int: ", "Per: ", "Dex: ", "Cha: "};
        Unit* p = player->getUnit();
        renderText("  HP: " + its(p->getStatValue(S_HP)) + "/" + its(p->getStatValue(S_MAXHP)), 2, loff + WIN1_WIDTH, toff, Z_MENU + 1, LEFT, forest);
        renderText("  MANA: " + its(p->getStatValue(S_MANA)) + "/" + its(p->getStatValue(S_MAXMANA)), 2, loff + WIN1_WIDTH, toff + 20, Z_MENU + 1, LEFT, navy);
        for (int i = S_STR; i <= S_CHA; i++) {
            int base = p->getStatValue(i - 9);
            int main = p->getStatValue(i);
            string s;
            if (main != base) s = statNames[i - S_STR] + "\\p" + its(main) + "\\z(" + its(base) + ")";
            else s = statNames[i - S_STR] + "\\z" + its(main);
            renderText(s, 2, loff + WIN1_WIDTH, toff + (i - S_STR + 2) * 20, Z_MENU + 1, LEFT, black);
        }
        renderText("Defense: \\q" + its(p->getStatValue(S_DEFENSE)), 2, loff + WIN1_WIDTH, toff + 200, Z_MENU + 1, LEFT, black);
        renderText("Exp pool: \\q" + its(player->getXpBank()), 2, loff + WIN1_WIDTH, toff + 240, Z_MENU + 1, LEFT, dark(teal));
        renderText("Time passed: \\q" + its(player->getUnit()->theTime), 2, loff + WIN1_WIDTH, toff + 260, Z_MENU + 1, LEFT, dark(red));
    } else if (topPanel == PANEL_SKILLS) {
        static const int numFunctionalSkills = 7;
        static const SkillType functionalSkills[] = {SKL_MELEE, SKL_LIFT, SKL_FORT, SKL_CONC, SKL_DODGE, SKL_RANGE, SKL_CRIT};
        int soff = WIN1_WIDTH + SWIN_WIDTH - 4;
        int k = 0;
        for (int i = 0; i < numFunctionalSkills; i++) {
            int level = player->getSkillLevel(functionalSkills[i]);
            if (level) {
                renderText(capitalize(skillNames[functionalSkills[i]]), 2, WIN1_WIDTH + loff, toff + k * 20, Z_MENU + 2, LEFT, dark(olive));
                int eve = player->getSkillExpPercent(functionalSkills[i]);
                string s = " (";
                if (eve < 10) s = "  (";
                int leve = level / 10;
                int evel = level % 10;
                renderText(its(leve) + "." + its(evel) + s + its(eve) + "%)", 2, soff, toff + k * 20, Z_MENU + 2, RIGHT, dark(olive));
                k++;
            }
        }
        for (map<int, playerSpell>::iterator i = player->getSpellsBegin(); i != player->getSpellsEnd(); i++) {
            int level = i->second.level;
            if (level) {
                int spellIndex = i->first << 2;
                renderText(capitalize(getAbility(spellIndex)->getName()), 2, WIN1_WIDTH + loff, toff + k * 20, Z_MENU + 2, LEFT, dark(teal));
                int eve = i->second.exp * 100 / (int)(pow(level + 1, 1.1) * 2.f);
                string s = " (";
                if (eve < 10) s = "  (";
                int leve = level / 10;
                int evel = level % 10;
                renderText(its(leve) + "." + its(evel) + s + its(eve) + "%)", 2, soff, toff + k * 20, Z_MENU + 2, RIGHT, dark(teal));
                k++;
            }
        }
        if (!k) {
            renderText("I'm sorry, but you currently", 2, WIN1_WIDTH + loff, toff, Z_MENU + 2, LEFT, black);
            renderText("lack any skill levels.", 2, WIN1_WIDTH + loff, toff + 20, Z_MENU + 2, LEFT, black);
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
                Location* loc = z->getLocationAt(i, j);
                color c;
                if (player->getUnit()->x == i && player->getUnit()->y == j) {
                    c = magenta;
                } else if (visibilities[j * z->getWidth() + i] < 2) {
                    pair<int, int> p = player->getMemoryBottom(i, j);
                    if (p.first == 5) {
                        c = black;
                    } else {
                        if (loc->structure == S_STAIRUP || loc->structure == S_STAIRDOWN) {
                            c = silver;
                        } else if (isClosedDoor(loc->structure) || isOpenDoor(loc->structure)) {
                            c = brown;
                        } else {
                            c.red = loc->height * 2;
                            c.green = loc->height * 2;
                            c.blue = loc->height * 2;
                        }
                    }
                } else if (loc->hasUnit()) {
                    c = red;
                } else if (loc->structure != S_NONE) {
                    if (loc->structure == S_STAIRUP || loc->structure == S_STAIRDOWN) {
                        c = silver;
                    } else if (isOpenDoor(loc->structure)) {
                        c = dark(tann);
                    } else if (isClosedDoor(loc->structure)) {
                        c = brown;
                    } else if (loc->structure == S_ROCK) {
                        c = charcoal;
                    } else {
                        c = pink;
                    }
                } else if (loc->hasItems()) {
                    c = green;
                } else {
                    c.red = loc->height * 3 + 32;
                    c.green = loc->height * 3 + 32;
                    c.blue = loc->height * 3 + 32;
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

        renderText(player->getZone()->getName(), 2, loff + WIN1_WIDTH, SWIN_HEIGHT / 2 + toff + 240, Z_MENU + 1, LEFT, charcoal);
    } else if (botPanel == PANEL_NOTES) {
        for (int i = 0; i < NUM_NOTELINES; i++) {
            renderText(theNotes[i], 2, WIN1_WIDTH + loff, i * 12 + SWIN_HEIGHT / 2 + 30, Z_MENU + 1, LEFT, black);
        }
        if (notesSelected && notesSelected < NUM_NOTELINES) {
            if (interval % 32 < 16) {
                drawBox(WIN1_WIDTH + 8 * theNotes[notesSelected].size() + loff - 4, notesSelected * 12 + SWIN_HEIGHT / 2 + 28, Z_MENU + 2, 1, 0, black);
            }
        }
    }
}

void Start::drawMenuBox(int x1, int y1, int x2, int y2) {
    int cWid = x2 - x1 - 32;
    int cHei = y2 - y1 - 32;
    ragd.drawTileSuperSpe(x1     , y1     , Z_MENU, 16  , 16  , getMenuTex(), 0 , 32, 16, 16);
    ragd.drawTileSuperSpe(x1 + 16, y1     , Z_MENU, cWid, 16  , getMenuTex(), 16, 32, 16, 16);
    ragd.drawTileSuperSpe(x2 - 16, y1     , Z_MENU, 16  , 16  , getMenuTex(), 32, 32, 16, 16);
    ragd.drawTileSuperSpe(x1     , y1 + 16, Z_MENU, 16  , cHei, getMenuTex(), 0 , 48, 16, 16);
    ragd.drawTileSuperSpe(x1 + 16, y1 + 16, Z_MENU, cWid, cHei, getMenuTex(), 16, 48, 16, 16);
    ragd.drawTileSuperSpe(x2 - 16, y1 + 16, Z_MENU, 16  , cHei, getMenuTex(), 32, 48, 16, 16);
    ragd.drawTileSuperSpe(x1     , y2 - 16, Z_MENU, 16  , 16  , getMenuTex(), 0 , 64, 16, 16);
    ragd.drawTileSuperSpe(x1 + 16, y2 - 16, Z_MENU, cWid, 16  , getMenuTex(), 16, 64, 16, 16);
    ragd.drawTileSuperSpe(x2 - 16, y2 - 16, Z_MENU, 16  , 16  , getMenuTex(), 32, 64, 16, 16);
}

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
    static const string dTypeNames[] = {"none", "bludgeon", "slashing", "piercing", "maaaaagic"};
    if (state == STATE_MENU) {
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
            graphic g = itemType->getGraphic(items[k].quantityCharge);
            int d = offset + 64 + 28 * i;
            ragd.drawTile(28 + 8 * (k % 2), d, Z_MENU + i + 1, getTexture(g.tex), g.loc);
            renderText(capitalize(itemType->getName()), 2, 65 + 8 * (k % 2), d + 10, Z_MENU + i + 1, LEFT, black);
            if (items[k].quantityCharge > 1 && typeStacks[itemType->getType()]) {
                renderText(its(items[k].quantityCharge), 1, 50 + 8 * (k % 2), d + 16, Z_MENU + i + 2, LEFT, black);
            }
        }

        ragd.drawTileSpe(8, curArrowY, Z_MENU + 20, getMenuTex(), arrowX[(interval % 40) / 4], arrowY[(interval % 40) / 4], 16);
        if (selectedShift > 0) {
            ragd.drawTileSuperSpe(IM_WID / 2 + 4, offset          + abs(interval % 24 / 2 - 6) + 36, Z_MENU, 24, 16, getMenuTex(), 48, 48, 32, -16);
        }
        if (numItems - selectedShift > MAX_MENU_ITEMS) {
            ragd.drawTileSuperSpe(IM_WID / 2 + 4, offset + height - abs(interval % 24 / 2 - 6) - 10, Z_MENU, 24, 16, getMenuTex(), 48, 32, 32,  16);
        }
        glColor4f(1, 1, 1, .5);
        ragd.drawTileSuperSpe(2, offset + 18, Z_MENU, IM_WID + 36, 16, getMenuTex(), 0, 80, 64, 16);
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

                //first it displays weight and value
                int weightValue = selectedItemType->getStatValue(S_WEIGHT);
                lines.push_back(pair<string, color>(" Weight: " + its(weightValue) + " peb.", weightColors[numDigits0(weightValue)]));
                int valueValue = selectedItemType->getStatValue(S_VALUE);
                lines.push_back(pair<string, color>(" Value: " + its(valueValue) + " cp", valueColors[numDigits0(valueValue)]));

                //then it displays damage and AC
                color c;
                c.alpha = 0;
                if (selectedItemType->hasStat(S_IDAMAGE, false)) {
                    int damVal = selectedItemType->getStatValue(S_IDAMAGE);
                    c.red = 2 * min(damVal, 32);
                    c.blue = 2 * min(damVal, 32);
                    c.green = 2 * min(damVal, 32);
                    lines.push_back(pair<string, color>(" " + its(damVal) + " Damage (" + capitalize(dTypeNames[weapDamTypes[selectedItemType->getStatValue(S_DTYPE)]]) + ")", c));
                }
                if (selectedItemType->hasStat(S_AC, false)) {
                    int acVal = selectedItemType->getStatValue(S_AC);
                    c.red = 2 * min(acVal, 32);
                    c.blue = 2 * min(acVal, 32);
                    c.green = 2 * min(acVal, 32);
                    lines.push_back(pair<string, color>(" " + its(acVal) + " AC", c));
                }

                //then it displays all other visible stats
                for (int i = SHOWN_ITEM_STATS_MIN; i <= SHOWN_ITEM_STATS_MAX; i++) {
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

                lines.push_back(pair<string, color>("", black));
                //then it displays any abilities it may have
                for (set<unsigned short>::iterator i = selectedItemType->getAbilitiesBegin(); i != selectedItemType->getAbilitiesEnd(); i++) {
                    lines.push_back(pair<string, color>("  " + capitalize(getAbility(*i)->getName()), dark(teal)));
                }
            }
            height = 32 + 30 + 12 * lines.size();
            offset = (WIN1_HEIGHT - height) / 2;
            drawMenuBox(IM_WID + 40, offset, IM_WID + 232, offset + height);
            glColor4f(1, 1, 1, .5);
            int nLen = selectedItemType->getName().size();
            ragd.drawTileSuperSpe(IM_WID + 132 - nLen * 7, offset + 9, Z_MENU + 1, 14 * nLen + 8, 32, getMenuTex(), 0, 80, 64, 16);
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
    ragd.drawTileSuperSpe(0, WIN1_HEIGHT + 4, Z_MENU, 32, 96, getMenuTex(), 0, 96, 32, 96);
    int x;
    for (x = 32; x < CWIN_WIDTH - 64; x += 32) {
        ragd.drawTileSuperSpe(x, WIN1_HEIGHT + 4, Z_MENU, 32, 96, getMenuTex(), 32, 96, 32, 96);
    }
    ragd.drawTileSuperSpe(x, WIN1_HEIGHT + 4, Z_MENU, CWIN_WIDTH - x - 32, 96, getMenuTex(), 32, 96, CWIN_WIDTH - x - 32, 96);
    ragd.drawTileSuperSpe(CWIN_WIDTH - 32, WIN1_HEIGHT + 4, Z_MENU, 32, 96, getMenuTex(), 64, 96, 32, 96);

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

typedef struct {
    int loc;
    int tex;
    float darkness;
} rendaten;

bool is_to_be_deleted(Unit* u) {
    return !(u->g.border);
}

void Start::renderGround() {

    Zone* z = player->getZone();
    int x = player->getUnit()->x;
    int y = player->getUnit()->y;

    glPushMatrix();
    glTranslatef(-(int)ragd.camX + WIN1_WIDTH / 2 - 16, -(int)ragd.camY + WIN1_HEIGHT / 2 - 16, 0);

    ragd.camX = x * TILE_SIZE;
    ragd.camY = y * TILE_SIZE;

    int maxXTiles = WIN1_WIDTH / TILE_SIZE;
    int maxYTiles = WIN1_HEIGHT / TILE_SIZE;
    int iMin = max(0, x - maxXTiles / 2 - 1);
    int jMin = max(0, y - maxYTiles / 2 - 2);
    int iMax = min(z->getWidth(), x + maxXTiles / 2 + 1);
    int jMax = min(z->getHeight(), y + maxYTiles / 2 + 2);

    raga.updateAnims();
    unitDeleteList.erase(remove_if(unitDeleteList.begin(), unitDeleteList.end(), is_to_be_deleted), unitDeleteList.end());

    rendaten renderAtEnd[(iMax - iMin) * (jMax - jMin)];
    int tot = 0;
    for (int i = iMin; i < iMax; i++) {
        for (int j = jMin; j < jMax; j++) {
            renderAtEnd[tot] = {-1, -1, -1};
            tot++;
            int locX = i * TILE_SIZE;
            int locY = j * TILE_SIZE;
            bool isMemory = false;
            if (visibilities[i + j * z->getWidth()] < 2) {
                pair<int, int> botMems = player->getMemoryBottom(i, j);
                if (botMems.first == 5) {
                    ragd.drawColorBox(locX, locY, Z_EFFECT - 1, locX + TILE_SIZE, locY + TILE_SIZE, black);
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
            float darkness = (double)(h*2 + 16) / (MAX_HEIGHT*3);

            if (isMemory) {
                darkness /= 2;
            }
            glColor4f(darkness, darkness, darkness, 1);
            Tile* tile = z->getTileAt(i, j);
            graphic g = tile->getGraphic();
            if (g.type == TT_OVER) {
                renderAtEnd[tot - 1] = {g.loc, g.tex, darkness};
                tile = tile->getOver();
                g = tile->getGraphic();
            }
            Texture* tex = getTexture(g.tex);
            int TZ;
            if (isMemory) {
                TZ = Z_EFFECT - 3;
            } else {
                TZ = Z_GROUND;
            }
            if (g.type == TT_NORMAL) {
                ragd.drawTile(locX, locY, TZ, tex, g.loc);
            } else if (g.type == TT_SMOOTH || g.type == TT_SMOOTHDOWN || g.type == TT_SMOOTHUP) {
                int b = g.border;
                int wid = tex->width / TILE_SIZE;
                int x1 = (g.loc % wid) * TILE_SIZE;
                int y1 = (g.loc / wid) * TILE_SIZE;

                Tile* tiles[8] = {z->safeGetTileAt(i-1, j-1), z->safeGetTileAt(i, j-1), z->safeGetTileAt(i+1, j-1),
                                z->safeGetTileAt(i-1, j), z->safeGetTileAt(i+1, j),
                                z->safeGetTileAt(i-1, j+1), z->safeGetTileAt(i, j+1), z->safeGetTileAt(i+1, j+1)};

                bool sm[8];
                for (int k = 0; k < 8; k++) {
                    sm[k] = tiles[k]->getGraphic().border == b && tiles[k] != tile;
                }

                if (g.type == TT_SMOOTHUP || g.type == TT_SMOOTHDOWN) {
                    int heights[8] = {z->safeGetLocationAt(i-1, j-1)->height, z->safeGetLocationAt(i, j-1)->height, z->safeGetLocationAt(i+1, j-1)->height,
                                        z->safeGetLocationAt(i-1, j)->height, z->safeGetLocationAt(i+1, j)->height,
                                        z->safeGetLocationAt(i-1, j+1)->height, z->safeGetLocationAt(i, j+1)->height, z->safeGetLocationAt(i+1, j+1)->height};
                    for (int k = 0; k < 8; k++) {
                        if ((heights[k] >= h - 2 && g.type == 2) || (heights[k] <= h + 2 && g.type == 3)) {
                            sm[k] = false;
                        }
                    }
                }
                if (sm[3] && sm[1]) {
                    ragd.drawTileSpe(locX            , locY, TZ, tex, x1, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[3] && sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (sm[3] && !sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, tex, x1, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                } else if (!sm[3] && sm[0] && !sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, tex, x1 + TILE_SIZE, y1, TILE_SIZE / 2);
                } else if (!sm[3] && !sm[0] && !sm[1]) {
                    ragd.drawTileSpe(locX, locY, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                }
                if (sm[1] && sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[1] && sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                } else if (sm[1] && !sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE, TILE_SIZE / 2);
                } else if (!sm[1] && sm[2] && !sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1, TILE_SIZE / 2);
                } else if (!sm[1] && !sm[2] && !sm[4]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 2, TILE_SIZE / 2);
                }
                if (sm[4] && sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[4] && sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (sm[4] && !sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                } else if (!sm[4] && sm[7] && !sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE * 3 / 2, y1 + TILE_SIZE / 2, TILE_SIZE / 2);
                } else if (!sm[4] && !sm[7] && !sm[6]) {
                    ragd.drawTileSpe(locX + TILE_SIZE / 2, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE / 2, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                }
                if (sm[6] && sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[6] && sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                } else if (sm[6] && !sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 5 / 2, TILE_SIZE / 2);
                } else if (!sm[6] && sm[5] && !sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE / 2, TILE_SIZE / 2);
                } else if (!sm[6] && !sm[5] && !sm[3]) {
                    ragd.drawTileSpe(locX, locY + TILE_SIZE / 2, TZ, tex, x1 + TILE_SIZE, y1 + TILE_SIZE * 3 / 2, TILE_SIZE / 2);
                }
            }
            if (isMemory) {
                glColor4f(.5, .5, .5, 1);
                pair<int, int> botMems = player->getMemoryBottom(i, j);
                if (botMems.first != 7 && botMems.first != 5) {
                    ragd.drawTile(locX, locY, Z_EFFECT - 2, getTexture(botMems.first), botMems.second);
                }
                pair<int, int> topMems = player->getMemoryTop(i, j);
                if (topMems.first != 7 && botMems.first != 5) {
                    ragd.drawTile(locX, locY, Z_EFFECT - 1, getTexture(topMems.first), topMems.second);
                }
            } else {
                glColor4f(1, 1, 1, 1);
                int struc = loc->structure;
                if (struc != S_NONE) {
                    ragd.drawTile(locX, locY, Z_STRUCT, getStructureTex(), struc);
                }

                glColor4f(1, 1, 1, 0.5);
                if (splatters[i + j * z->getWidth()] < 255) {
                    ragd.drawTile(locX, locY, Z_SPLAT, getSplatterTex(), splatters[i + j * z->getWidth()]);
                }
                glColor4f(1, 1, 1, 1);
                if (loc->hasItems()) {
                    Item* items = &(*loc->items)[0];
                    int numItems = loc->items->size();
                    for (int k = 0; k < numItems; k++) {
                        ItemType* itemType = getItemType(items[k].itemType);
                        graphic g = itemType->getGraphic(items[k].quantityCharge);
                        ragd.drawTile(locX, locY, Z_ITEM + k, getTexture(g.tex), g.loc);
                    }
                }
                if (loc->hasUnit()) {
                    raga.unitAnimTest(loc->unit, locX, locY);
                }
            }
        }
    }
    glColor4f(1, 1, 1, 1);

    raga.renderAnims();

    tot = 0;
    for (int i = iMin; i < iMax; i++) {
        for (int j = jMin; j < jMax; j++) {
            if (renderAtEnd[tot].loc != -1) {
                int locX = i * TILE_SIZE;
                int locY = j * TILE_SIZE;
                float darkness = renderAtEnd[tot].darkness;
                glColor4f(darkness, darkness, darkness, 1);
                ragd.drawTile(locX, locY, Z_FOREGROUND, getTexture(renderAtEnd[tot].tex), renderAtEnd[tot].loc);
            }
            tot++;
        }
    }
    glColor4f(1, 1, 1, 1);
    drawBox(x + 50, y + 50, Z_EFFECT, 4, interval, scarlet);
    updateEffects(x, y);
    if (state == STATE_TARGET) {
        //51-55
        //67-71
        int num = (interval % 40) / 4;
        if (num >= 5) num += 62;
        else num += 51;
        ragd.drawTile(unitsInRange[stIndex]->x * TILE_SIZE, unitsInRange[stIndex]->y * TILE_SIZE, Z_MENU, getMenuTex(), num);
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

	ragd = RagDrawer(TILE_SIZE, player);
	raga = RagAnim(&ragd);

	createEffect(P_ARROW, 0, 0);
}

//0-tiny, 1-small, 2-normal, 3-bold, 4-skinny, 5-large
int fontWid[] = {4, 6, 8 , 8, 6, 14};
int fontHei[] = {6, 9, 12, 12, 12, 20};
int offX[] = {  0, 132, 0, 128, 132, 266};
int offY[] = {192,   0, 0, 157, 72 ,   0};
int numX[] = {32, 16, 16, 16, 20, 16};

//                    a      b     c     d           e       f        g      h          i       j     k     l     m        n     o       p       q      r    s       t     u     v       w      x     y       z
color textColors[] = {azure, blue, cyan, chartreuse, forest, fuchsia, green, harlequin, indigo, jade, rose, lime, magenta, navy, orange, purple, brown, red, salmon, tann, gray, violet, white, grey, yellow, black};
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
    int j = 0;
    for (unsigned int i = 0; i < text.size(); i++, j++) {
        if (text[i] == '\\') {
            i++; j--;
            color newC = textColors[text[i] - 'a'];
            glColor3f(newC.red / 255., newC.green / 255., newC.blue / 255.);
        } else {
            ragd.drawTileSuperSpe(x + j * w - a, y, z, w, h, getFontTex(), offX[size] + text[i] % numX[size] * w, offY[size] + text[i] / numX[size] * h, w, h);
        }
    }

    glColor3f(1, 1, 1);
}
