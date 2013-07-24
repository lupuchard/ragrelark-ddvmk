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


static const String FOOD_TASTES[] = {"It tastes rather bland.", "It was delicious!", "Hmm, pretty good.", "Salty.", "Yikes! It's very spicy.", "It tastes terrible.", "Wow that's really sour why did you just eat that.", "The taste is divine. You shed a tear."};

void Start::directionPress(int direction) {
    switch(state) {
    case STATE_PLAY:
        if (direction == 5) {
            search(player->getUnit(), player->getZone());
        } else {
            moveUnit(player->getUnit(), player->getZone(), direction);
            primeFolder->getGround()->setLocation(player->getZone(), player->getUnit()->pos);
        } break;
    case STATE_MENU:
        switch (direction) {
            case 2:
                selected++;
                if (selected >= folderStack.top()->getNumItems()) {
                    selected = 0;
                } break;
            case 4:
                menuAction--;
                if (menuAction < 0) {
                    menuAction = NUM_MENU_ACTIONS - 1;
                } break;
            case 6:
                menuAction++;
                if (menuAction >= NUM_MENU_ACTIONS) {
                    menuAction = 0;
                } break;
            case 8:
                selected--;
                if (selected < 0) {
                    selected = folderStack.top()->getNumItems() - 1;
                } break;
        } break;
    case STATE_TARGET:
        switch (direction) {
            case 6:
                stIndex++;
                if (stIndex >= (int)unitsInRange.size()) {
                    stIndex = 0;
                } break;
            case 4:
                stIndex--;
                if (stIndex < 0) {
                    stIndex = unitsInRange.size() - 1;
                } break;
            default: break;
        } break;
    case STATE_SPELL: circleSelect[direction] = !circleSelect[direction]; break;
    default: break;
    }
}

void Start::events() {
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: done = true; break;
            case SDL_MOUSEBUTTONDOWN: {
                if (notesSelected) {
                    SDL_EnableUNICODE(false);
                    notesSelected = 0;
                }
                int x = event.button.x;
                int y = event.button.y;
                if (x > WIN1_WIDTH) {
                    if (botPanel == PANEL_NOTES && y > SWIN_HEIGHT / 2 + 30) {
                        notesSelected = std::max((y - SWIN_HEIGHT / 2 - 26) / 12, 1);
                        SDL_EnableUNICODE(true);
                    } else if (y > SWIN_HEIGHT / 2 && y <= SWIN_HEIGHT / 2 + 30) {
                        int k = 0;
                        for (int i = PANEL_BOTTOMSTART + 1; i < PANEL_BOTTOMEND; i++) {
                            if (x > WIN1_WIDTH + k * 65 && x < WIN1_WIDTH + (k + 1) * 65) {
                                botPanel = i;
                                break;
                            }
                            k++;
                        }
                    } else if (y <= 30) {
                        int k = 0;
                        for (int i = PANEL_TOPSTART + 1; i < PANEL_TOPEND; i++) {
                            if (x > WIN1_WIDTH + k * 65 && x < WIN1_WIDTH + (k + 1) * 65) {
                                topPanel = i;
                                break;
                            }
                            k++;
                        }
                    }
                } }break;
            case SDL_KEYDOWN:
            if (notesSelected && notesSelected < NUM_NOTELINES) {
                int k = event.key.keysym.sym;
                if (k == SDLK_UP) {
                    notesSelected--;
                } else if (k == SDLK_BACKSPACE || k == SDLK_DELETE) {
                    if (theNotes[notesSelected].empty()) notesSelected--;
                    else {
                        theNotes[notesSelected].erase(theNotes[notesSelected].size() - 1, 1);
                    }
                } else if (k == SDLK_RETURN || k == SDLK_DOWN) {
                    notesSelected++;
                } else if (k == SDLK_TAB) {
                    theNotes[notesSelected] += "   ";
                } else {
                    if (k != SDLK_LSHIFT && k != SDLK_RSHIFT && k != SDLK_LCTRL && k != SDLK_RCTRL && k != SDLK_CAPSLOCK && k != SDLK_LALT && k != SDLK_RALT) {
                        theNotes[notesSelected] += event.key.keysym.unicode;
                    }
                }
                if (notesSelected < NUM_NOTELINES && theNotes[notesSelected].size() >= 28) {
                    notesSelected++;
                }
            } else if (shiftIsDown) {
                switch(event.key.keysym.sym) {
                    case SDLK_COMMA:  goTheStairs(player->getUnit(), player->getZone()); break;
                    case SDLK_PERIOD: goTheStairs(player->getUnit(), player->getZone()); break;

                    case SDLK_i: openInventory(); menuAction = MA_EXAMINE; break;
                    case SDLK_g: groundGrab(); break;
                    case SDLK_e: openEquipment(); menuAction = MA_EXAMINE;   break;
                    case SDLK_q: openBag();       menuAction = MA_EQUIP;   break;
                    case SDLK_u: openEquipment(); menuAction = MA_GRAB;    break;
                    case SDLK_c: closeDoors(); break;
                    case SDLK_s: search(player->getUnit(), player->getZone()); break;

                    case SDLK_d: player->getUnit()->print(); break;
                    default: break;
                }
            } else {
                switch(event.key.keysym.sym) {
                    case SDLK_KP1:   directionPress(1); break;
                    case SDLK_KP2:   directionPress(2); break;
                    case SDLK_KP3:   directionPress(3); break;
                    case SDLK_KP4:   directionPress(4); break;
                    case SDLK_KP5:   directionPress(5); break;
                    case SDLK_KP6:   directionPress(6); break;
                    case SDLK_KP7:   directionPress(7); break;
                    case SDLK_KP8:   directionPress(8); break;
                    case SDLK_KP9:   directionPress(9); break;
                    case SDLK_UP:    directionPress(8); break;
                    case SDLK_DOWN:  directionPress(2); break;
                    case SDLK_LEFT:  directionPress(4); break;
                    case SDLK_RIGHT: directionPress(6); break;
                    case SDLK_j:   directionPress(1); break;
                    case SDLK_k:   directionPress(2); break;
                    case SDLK_l:   directionPress(3); break;
                    case SDLK_u:   directionPress(4); break;
                    case SDLK_i:   directionPress(5); break;
                    case SDLK_o:   directionPress(6); break;
                    case SDLK_7:   directionPress(7); break;
                    case SDLK_8:   directionPress(8); break;
                    case SDLK_9:   directionPress(9); break;

                    case SDLK_s:      openInventory(); menuAction = MA_EXAMINE; break;
                    case SDLK_PERIOD: openInventory(); menuAction = MA_EXAMINE; break;
                    case SDLK_x:      openBag();       menuAction = MA_EXAMINE; break;
                    case SDLK_w:      openGround();    menuAction = MA_GRAB;    break;
                    case SDLK_COMMA:  groundGrab();    menuAction = MA_GRAB;    break;
                    case SDLK_d:      openBag();       menuAction = MA_DROP;    break;
                    case SDLK_e:      openBag();       menuAction = MA_EAT;     break;

                    case SDLK_f:
                        if (state == STATE_TARGET) state = STATE_PLAY;
                        else {
                            stateAction = SA_FIRE;
                            enterTargetMode();
                        } break;
                    case SDLK_z:
                    case SDLK_SEMICOLON: //two options for the same thing
                        if (state == STATE_SPELL) state = STATE_PLAY;
                        else enterSpellMode();
                        break;
                    case SDLK_LSHIFT: shiftIsDown = true; break;
                    case SDLK_RSHIFT: shiftIsDown = true; break;
                    case SDLK_ESCAPE: state = STATE_PLAY; selected = 0; break;
                    case SDLK_SPACE:  enterCommand(); break;
                    case SDLK_RETURN: enterCommand(); break;
                    case SDLK_LCTRL: backCommand(); break;
                    case SDLK_RCTRL: backCommand(); break;
                    default: break;
                }
            } break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_LSHIFT: shiftIsDown = false; break;
                    case SDLK_RSHIFT: shiftIsDown = false; break;
                    default: break;
                }

            default: break;
        }
    }
}

void Start::closeDoors() {
    int dir = 0;
    Zone* zone = player->getZone();
    Unit* u = player->getUnit();
    for (int i = 1; i < NUM_COORD_DIRS; i++) {
        Location* loc = zone->getLocationAt(u->pos + DIRS[i]);
        if (loc->isOpenDoor()) {
            if (dir > 0) {
                dir = -1;
                break;
            }
            dir = i;
        }
    }
    if (dir == 0) {
        addMessage("There are no open doors around to close.", GRAY);
    } else if (dir != -1) {
        closeDoor(u, zone, dir, true);
    }
}

void Start::groundGrab() {
    if (primeFolder->getGround()->getNumItems() == 1) {
        Item item = primeFolder->getGround()->removeItem(selected);
        if (!item.getType()->isSlot()) {
            if (!primeFolder->getBag()->addItem(&item)) {
                primeFolder->getGround()->addItem(&item);
            } else {
                addMessage("You pick up " + item.getName() + ".", BLACK);
            }
        }
        player->getUnit()->theTime += 5;
        itemRemovalCheck();
    } else {
        openGround();
        menuAction = MA_GRAB;
    }
}

void Start::openInventory() {
    if (state != STATE_DEAD) {
        while(folderStack.top() != primeFolder) {
            folderStack.pop();
        }
        state = STATE_MENU;
    }
}

void Start::openBag() {
    if (primeFolder->getBag()->getNumItems() > 0) {
        while(folderStack.top() != primeFolder) {
            folderStack.pop();
        }
        folderStack.push(primeFolder->getBag());
        state = STATE_MENU;
    }
}

void Start::openEquipment() {
    while(folderStack.top() != primeFolder) {
        folderStack.pop();
    }
    folderStack.push(primeFolder->getEquips());
    state = STATE_MENU;
}

void Start::openGround() {
    if (primeFolder->getGround()->getNumItems() > 0) {
        while(folderStack.top() != primeFolder) {
            folderStack.pop();
        }
        folderStack.push(primeFolder->getGround());
        state = STATE_MENU;
    }
}

void Start::enterTargetMode() {
    unitsInRange.clear();
    Zone* z = player->getZone();
    for (int j = 0; j < z->getHeight(); j++) {
        for (int i = 0; i < z->getWidth(); i++) {
            int v = i + j * z->getWidth();
            if (visibilities[v] == 2) {
                Location* loc = z->getLocationAt(v);
                if (loc->hasUnit() && loc->unit != player->getUnit()) {
                    unitsInRange.push_back(loc->unit);
                }
            }
        }
    }
    if (!unitsInRange.empty()) {
        state = STATE_TARGET;
        stIndex = 0;
    } else {
        addMessage("There are no targets in range!", BLACK);
    }
}

void Start::enterSpellMode() {
    state = STATE_SPELL;
    for (int i = 0; i < 10; i++) {
        circleSelect[i] = false;
    }
}

void Start::action(Skill* skill, int exp) {
    int toteExp = player->getUnit()->modifyStat(Stat::EXP, exp);
    int lev = player->gainSkillExp(skill, exp);
    if (lev) {
        int level = player->getSkillLevel(skill);
        int leve = level / 10;
        int evel = level % 10;
        if (lev > 0) {
            addMessage("Your " + skill->name + " has leveled up to " + its(leve) + "." + its(evel) + "!", FOREST);
        } else {
            addMessage("Your " + skill->name + " has gone down to " + its(leve) + "." + its(evel) + "!", MAROON);
        }
        std::set<Stat*> skillAfflictions = Stat::getSkillAfflictions(skill);
        for (std::set<Stat*>::iterator i = skillAfflictions.begin(); i != skillAfflictions.end(); ++i) {
            Stat* theStat = (Stat*)*i;
            player->getUnit()->needToUpdate(theStat->getIndex(), theStat->isItFloat());
        }
    }
    int expReq = player->getUnit()->getStatValue(Stat::EXPREQ);
    if (toteExp >= expReq) {
        player->getUnit()->modifyStat(Stat::EXP, -expReq);
        int lev = player->getUnit()->modifyStat(Stat::LEVEL, 1);
        static const String LEVEL_UP_WORDS[] = {"congrats", "wonderful", "sweet", "congradulations", "fantastic", "cool", "fabulous", "incredible", "awesome",
                        "amazing", "brilliant", "super", "great", "admirable", "exceptional", "marvelous", "terrific", "outstanding", "superb",
                        "epic", "woah", "right on", "crazy", "stupendous", "congratz", "astonishing", "beautiful", "breathtaking", "exalting",
                        "grand", "nice", "impressive", "neat", "phenomenal", "nifty", "wow", "fancy", "groovy", "spectacular",
                        "commendable", "favorable", "legendary", "swell", "sensational", "splended", "majestic", "wondrous", "acceptable", "fine, be that way"};
        addMessage("You leveled up! " + capitalize(LEVEL_UP_WORDS[std::min(lev - 2, 48)]) + "!", BLACK);
    }
}

bool Start::init() {
    // init sdl
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) return false;

    // create window
    SDL_WM_SetIcon(SDL_LoadBMP("icon.bmp"), NULL);
    SDL_WM_SetCaption("Ragrelark", "Ragrelark");
    display = SDL_SetVideoMode(WIN1_WIDTH + SWIN_WIDTH, WIN1_HEIGHT + CWIN_HEIGHT, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
    if(display == NULL) return false;
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    srand(time(NULL));

    return true;
}

void Start::sapExp(Unit* sapper, Unit* target, Skill* skill, int multitude) {
    if (sapper == player->getUnit()) {
        int expLeft = target->getStatValue(Stat::EXP);
        int bnk = player->getXpBank();
        if (bnk < 0) {
            int gain = std::min(expLeft, -bnk);
            expLeft -= gain;
            player->bankXp(gain);
        }
        if (expLeft) {
            int gain = std::min(expLeft, (int)target->getStatValue(Stat::LEVEL) + 1);
            action(skill, gain);
            target->modifyStat(Stat::EXP, -gain);
        }
    }
}

void Start::debankExp(Unit* debanker, Skill* skill, int amount) {
    if (debanker == player->getUnit()) {
        action(skill, player->takeFromXpBank(amount));
    }
}

bool Start::equipItem(Item item) {
    ItemType* itemType = ItemType::get(item.itemType);
    ItemSlot* typeSlot = itemType->getSlot();
    if (typeSlot) {
        std::list<Item> toBeRemoved;
        primeFolder->getEquips()->equipItem(item, toBeRemoved);
        for (std::list<Item>::iterator iter = toBeRemoved.begin(); iter != toBeRemoved.end(); ++iter) {
            if (!primeFolder->getBag()->addItem(&*iter)) {
                primeFolder->getGround()->addItem(&*iter);
            }
        }
        std::set<Stat*> itemAfflictions = Stat::getItemAfflictions();
        for (std::set<Stat*>::iterator i = itemAfflictions.begin(); i != itemAfflictions.end(); ++i) {
            Stat* theStat = (Stat*)*i;
            player->getUnit()->needToUpdate(theStat->getIndex(), theStat->isItFloat());
        }
        player->getUnit()->theTime += typeSlot->timeEquip;
        return true;
    }
    return false;
}

static const String LOAD_NAMES[] = {"", "Encumbered", "Heavily Encumbered", "Overloaded", "Overloaded (Stuck)"};
static const Color ENCUMBER_COLOR = {200, 100, 20};
void Start::itemRemovalCheck() {
    if (folderStack.top()->getNumItems() == 0) {
        folderStack.pop();
        selected = 0;
    } else if (selected >= folderStack.top()->getNumItems()) {
        selected--;
    }
    int totalWeight = primeFolder->getEquips()->getStatValue(Stat::WEIGHT);
    Item* bagItems = primeFolder->getBag()->getItems();
    int numItems = primeFolder->getBag()->getNumItems();
    for (int i = 0; i < numItems; i++) {
        totalWeight += ItemType::get(bagItems[i].itemType)->getStatValue(Stat::WEIGHT);
    }
    int lightLoad = player->getUnit()->getStatValue(Stat::LOAD);
    int prevLoad = loadStatus;
    for (loadStatus = 0; (loadStatus + 1) * (lightLoad + 1) < totalWeight; loadStatus++);
    if (loadStatus > 4) loadStatus = 4;
    if (prevLoad != loadStatus) {
        if (loadStatus > 0) {
            addStatus(LOAD_NAMES[loadStatus], ENCUMBER_COLOR, ST_ENCUM);
        } else {
            removeStatus(ST_ENCUM);
        }
    }
}

void Start::enterCommand() {
    if (state == STATE_MENU) {
        Item selectedItem = *folderStack.top()->getItem(selected);
        if (selectedItem.getType()->isFolder()) {
            ItemFolder* folder = getItemFolder(selectedItem);
            if (folder->getNumItems() > 0) {
                folderStack.push(getItemFolder(selectedItem));
                selected = 0;
            }
        } else {
            switch(menuAction) {
                case MA_GRAB: {
                    Item item = folderStack.top()->removeItem(selected);
                    if (!item.getType()->isSlot()) {
                        if (!primeFolder->getBag()->addItem(&item)) {
                            primeFolder->getGround()->addItem(&item);
                        } else {
                            if (folderStack.top() == primeFolder->getGround()) {
                                addMessage("You pick up " + item.getName() + ".", BLACK);
                            } else if (folderStack.top() == primeFolder->getEquips()) {
                                addMessage("You unequip " + item.getName() + ".", BLACK);
                            }
                        }
                    }
                    player->getUnit()->theTime += 4;
                    itemRemovalCheck();
                } break;
                case MA_DROP: {
                    Item item = folderStack.top()->removeItem(selected);
                    if (!item.getType()->isSlot()) {
                        addItemToPlace(player->getUnit()->pos, player->getZone(), item);
                        addMessage("You drop " + item.getName() + ".", BLACK);
                    }
                    player->getUnit()->theTime += 2;
                    itemRemovalCheck();
                } break;
                case MA_EQUIP: {
                    if (folderStack.top() != primeFolder->getEquips()) {
                        Item item = *folderStack.top()->getItem(selected);
                        if (equipItem(item)) {
                            folderStack.top()->removeItem(selected);
                            itemRemovalCheck();
                            addMessage("You equip " + item.getName() + ".", BLACK);
                        }
                    }
                } break;
                case MA_EAT: {
                    Item* item = folderStack.top()->getItem(selected);
                    ItemType* itemType = item->getType();
                    if (itemType->isEdible()) {
                        if (player->getUnit()->getStatValue(Stat::HUNGER) > MAX_HUNGER) {
                            addMessage("You are way too full to eat right now!", GRAY);
                        } else {
                            if (item->quantityCharge > 1) {
                                item->quantityCharge--;
                            } else {
                                folderStack.top()->removeItem(selected);
                                itemRemovalCheck();
                            }
                            eatFood(player->getUnit(), itemType);
                            addMessage("You eat the " + itemType->getName() + ". " + FOOD_TASTES[itemType->getStatValue(Stat::TASTE)], BLACK);
                        }
                    } else {
                        addMessage("That is not a food.", GRAY);
                    }
                } break;
                default: break;
            }
        }
    } else if (state == STATE_TARGET) {
        switch(stateAction) {
            case SA_FIRE: {
                Item* rangedItem = NULL;
                // if there is a ranged item equipped find out which item it is
                for (int i = 0; i < primeFolder->getEquips()->getNumItems(); i++) {
                    Item* item = primeFolder->getEquips()->getItem(i);
                    if (item->getType()->isRanged()) {
                        rangedItem = item;
                        break;
                    }
                }
                if (!rangedItem) break;
                // ...then check if theres any applicable ammo...
                Item* items = primeFolder->getBag()->getItems();
                for (int i = 0; i < primeFolder->getBag()->getNumItems(); i++) {
                    ItemType* itemType = items[i].getType();
                    if (itemType->getType() == rangedItem->getType()->getAmmo()) {
                        // ...and fire
                        primeFolder->getEquips()->setExtra(items[i]);
                        Item item = items[i];
                        if (items[i].quantityCharge > 1) { //m17
                            items[i].quantityCharge--;
                        } else {
                            primeFolder->getBag()->removeItem(i);
                            itemRemovalCheck();
                        }
                        item.quantityCharge = 1;
                        Unit* enemy = unitsInRange[stIndex];
                        addItemToPlace(enemy->pos, player->getZone(), item);
                        addProj(player->getUnit()->pos.x * TILE_SIZE, player->getUnit()->pos.y * TILE_SIZE, enemy->pos.x * TILE_SIZE, enemy->pos.y * TILE_SIZE, 10, 0);
                        shootUnit(player->getUnit(), *rangedItem, unitsInRange[stIndex], player->getZone());
                        primeFolder->getEquips()->removeExtra();
                        break;
                    }
                }
            } break;
            default: break;
        }
        state = STATE_PLAY;
    } else if (state == STATE_SPELL) {
        state = STATE_PLAY;
        unsigned int spellI = 0;
        for (int i = 9; i >= 0; i--) {
            int j;
            switch(i) {
                case 2: j = 1; break;
                case 8: j = 2; break;
                case 3: j = 3; break;
                case 5: j = 4; break;
                case 6: j = 6; break;
                case 1: j = 7; break;
                case 7: j = 8; break;
                case 4: j = 9; break;
                default: j = 0;
            }
            spellI = (spellI << 1) | circleSelect[j];
        }
        castSpell(spellI, player->getUnit(), player->getZone());
    }
}

void Start::backCommand() {
    if (state == STATE_MENU) {
        if (folderStack.top() != primeFolder) {
            folderStack.pop();
        } else {
            state = STATE_PLAY;
        }
        selected = 0;
    }
}
