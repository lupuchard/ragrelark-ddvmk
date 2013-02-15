#include "Start.h"

const string foodTastes[] = {"It tastes rather bland.", "It was delicious!", "Hmm, pretty good.", "Salty.", "Yikes! It's very spicy.", "It tastes terrible.", "Wow that's really sour why did you just eat that.", "The taste is divine. You shed a tear."};

/*
ok here is list

libsdl-image1.2
libsdl1.2debian

*/

void Start::directionPress(int direction) {
    switch(state) {
    case STATE_PLAY:
        if (direction == 5) {
            search(player->getUnit(), player->getZone());
        } else {
            moveUnit(player->getUnit(), player->getZone(), direction);
            primeFolder->getGround()->setLocation(player->getZone(), player->getUnit()->x, player->getUnit()->y);
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
                        notesSelected = max((y - SWIN_HEIGHT / 2 - 26) / 12, 1);
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
    for (int i = 1; i <= 9; i++) {
        int x = xDirs[i] + u->x;
        int y = yDirs[i] + u->y;
        Location* loc = zone->getLocationAt(x, y);
        if (loc->isOpenDoor()) {
            if (dir > 0) {
                dir = -1;
                break;
            }
            dir = i;
        }
    }
    if (dir == 0) {
        addMessage("There are no open doors around to close.", gray);
    } else if (dir != -1) {
        closeDoor(u, zone, dir, true);
    }
}

void Start::groundGrab() {
    if (primeFolder->getGround()->getNumItems() == 1) {
        Item item = primeFolder->getGround()->removeItem(selected);
        int itemTypeType = getItemType(item.itemType)->getType();
        if (!(itemTypeType == 0 || itemTypeType == I_SLOT)) {
            if (!primeFolder->getBag()->addItem(&item)) {
                primeFolder->getGround()->addItem(&item);
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
    while(folderStack.top() != primeFolder) {
        folderStack.pop();
    }
    state = STATE_MENU;
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
    for (int i = 0; i < z->getWidth(); i++) {
        for (int j = 0; j < z->getHeight(); j++) {
            int v = i + j * z->getWidth();
            if (visibilities[v] == 2) {
                Location* loc = z->getLocationAt(i, j);
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
        addMessage("There are no targets in range!", black);
    }
}

void Start::enterSpellMode() {
    state = STATE_SPELL;
    for (int i = 0; i < 10; i++) {
        circleSelect[i] = false;
    }
}

void Start::action(SkillType skill, int exp) {
    int toteExp = player->getUnit()->modifyStat(S_EXP, exp);
    int lev = player->gainSkillExp(skill, exp);
    if (lev) {
        int level = player->getSkillLevel(skill);
        int leve = level / 10;
        int evel = level % 10;
        if (lev > 0) {
            addMessage("Your " + skillNames[skill] + " has leveled up to " + its(leve) + "." + its(evel) + "!", forest);
        } else {
            addMessage("Your " + skillNames[skill] + " has gone down to " + its(leve) + "." + its(evel) + "!", maroon);
        }
        set<Stat*> skillAfflictions = getSkillAfflictions(skill);
        for (set<Stat*>::iterator i = skillAfflictions.begin(); i != skillAfflictions.end(); i++) {
            Stat* theStat = (Stat*)*i;
            player->getUnit()->needToUpdate(theStat->getIndex(), theStat->isItFloat());
        }
    }
    int expReq = player->getUnit()->getStatValue(S_EXPREQ);
    if (toteExp >= expReq) {
        player->getUnit()->modifyStat(S_EXP, -expReq);
        int lev = player->getUnit()->modifyStat(S_LEVEL, 1);
        const string levelUpWords[] = {"congrats", "wonderful", "sweet", "congradulations", "fantastic", "cool", "fabulous", "incredible", "awesome",
                        "amazing", "brilliant", "super", "great", "admirable", "exceptional", "marvelous", "terrific", "outstanding", "superb",
                        "epic", "woah", "right on", "crazy", "stupendous", "congratz", "astonishing", "beautiful", "breathtaking", "exalting",
                        "grand", "nice", "impressive", "wondrous", "phenomenal", "nifty", "wow", "fancy", "groovy", "spectacular",
                        "commendable", "favorable", "legendary", "swell", "sensational", "splended", "majestic", "wondrous", "acceptable", "fine, be that way"};
        addMessage("You leveled up! " + capitalize(levelUpWords[min(lev - 2, 48)]) + "!", black);
    }
}

bool Start::init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }
    display = SDL_SetVideoMode(WIN1_WIDTH + SWIN_WIDTH, WIN1_HEIGHT + CWIN_HEIGHT, 32, SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL);
    if(display == NULL) {
        return false;
    }
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    startRenderer();

    srand(time(NULL));

    initFieldOfView();
    return true;
}

void Start::sapExp(Unit* sapper, Unit* target, SkillType skill, int multitude) {
    if (sapper == player->getUnit()) {
        int expLeft = target->getStatValue(S_EXP);
        int bnk = player->getXpBank();
        if (bnk < 0) {
            int gain = min(expLeft, -bnk);
            expLeft -= gain;
            player->bankXp(gain);
        }
        if (expLeft) {
            int gain = min(expLeft, (int)target->getStatValue(S_LEVEL) + 1);
            action(skill, gain);
            target->modifyStat(S_EXP, -gain);
        }
    }
}

void Start::debankExp(Unit* debanker, SkillType skill, int amount) {
    if (debanker == player->getUnit()) {
        action(skill, player->takeFromXpBank(amount));
    }
}

bool Start::equipItem(Item item) {
    ItemType* itemType = getItemType(item.itemType);
    int typeSlot = typeSlots[itemType->getType()];
    if (typeSlot != E_NONE && typeSlot != E_AMMO) {
        Item oldItem = primeFolder->getEquips()->equipItem(item, itemType->getType());
        int oldItemTypeType = getItemType(oldItem.itemType)->getType();
        if (oldItemTypeType == I_SLOT) {
            if (oldItem.quantityCharge == 1) {
                return false;
            } else if (oldItem.quantityCharge == 2) {
                Item qux = primeFolder->getEquips()->removeItem(E_LHAND);
                Item quux = primeFolder->getEquips()->removeItem(E_RHAND);
                if (!primeFolder->getBag()->addItem(&qux)) primeFolder->getGround()->addItem(&qux);
                if (!primeFolder->getBag()->addItem(&quux)) primeFolder->getGround()->addItem(&quux);
                primeFolder->getEquips()->equipItem(item, itemType->getType());
            }
        } else if (oldItemTypeType != 0) {
            if (!primeFolder->getBag()->addItem(&oldItem)) {
                primeFolder->getGround()->addItem(&oldItem);
            }
        }
        if (typeSlots[itemType->getType()] == E_BAG) {
            primeFolder->getBag()->setCapacity(itemType->getStatValue(S_CAPACITY));
        }
        set<Stat*> itemAfflictions = getItemAfflictions();
        for (set<Stat*>::iterator i = itemAfflictions.begin(); i != itemAfflictions.end(); i++) {
            Stat* theStat = (Stat*)*i;
            player->getUnit()->needToUpdate(theStat->getIndex(), theStat->isItFloat());
        }
        player->getUnit()->theTime += timeEquip[typeSlot];
        return true;
    }
    return false;
}

const string loadNames[] = {"", "Encumbered", "Heavily Encumbered", "Overloaded", "Overloaded (Stuck)"};
const color encumberColor = {200, 100, 20};
void Start::itemRemovalCheck() {
    if (folderStack.top()->getNumItems() == 0) {
        folderStack.pop();
        selected = 0;
    } else if (selected >= folderStack.top()->getNumItems()) {
        selected--;
    }
    int totalWeight = primeFolder->getEquips()->getStatValue(S_WEIGHT);
    Item* bagItems = primeFolder->getBag()->getItems();
    int numItems = primeFolder->getBag()->getNumItems();
    for (int i = 0; i < numItems; i++) {
        totalWeight += getItemType(bagItems[i].itemType)->getStatValue(S_WEIGHT);
    }
    int lightLoad = player->getUnit()->getStatValue(S_LOAD);
    int prevLoad = loadStatus;
    for (loadStatus = 0; (loadStatus + 1) * (lightLoad + 1) < totalWeight; loadStatus++);
    if (loadStatus > 4) loadStatus = 4;
    if (prevLoad != loadStatus) {
        if (loadStatus > 0) {
            addStatus(loadNames[loadStatus], encumberColor, 22);
        } else {
            removeStatus(22);
        }
    }
}

void Start::enterCommand() {
    if (state == STATE_MENU) {
        Item selectedItem = *folderStack.top()->getItem(selected);
        int itemTypeType = getItemType(selectedItem.itemType)->getType();
        if (itemTypeType <= 14 && itemTypeType >= 10) {
            ItemFolder* folder = getItemFolder(selectedItem);
            if (folder->getNumItems() > 0) {
                folderStack.push(getItemFolder(selectedItem));
                selected = 0;
            }
        } else {
            switch(menuAction) {
                case MA_GRAB: {
                    Item item = folderStack.top()->removeItem(selected);
                    int itemTypeType = getItemType(item.itemType)->getType(); //an absolutely beautiful line of code
                    if (!(itemTypeType == 0 || itemTypeType == I_SLOT)) {
                        if (!primeFolder->getBag()->addItem(&item)) {
                            primeFolder->getGround()->addItem(&item);
                        }
                    }
                    player->getUnit()->theTime += 4;
                    itemRemovalCheck();
                } break;
                case MA_DROP: {
                    Item item = folderStack.top()->removeItem(selected);
                    int itemTypeType = getItemType(item.itemType)->getType();
                    if (!(itemTypeType == 0 || itemTypeType == I_SLOT)) {
                        addItemToPlace(player->getUnit()->x, player->getUnit()->y, player->getZone(), item);
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
                        }
                    }
                } break;
                case MA_EAT: {
                    Item* item = folderStack.top()->getItem(selected);
                    ItemType* itemType = getItemType(item->itemType);
                    if (itemType->getType() == 41) {
                        if (player->getUnit()->getStatValue(S_HUNGER) > MAX_HUNGER) {
                            addMessage("You are way too full to eat right now!", gray);
                        } else {
                            if (item->quantityCharge > 1) { //m17
                                item->quantityCharge--;
                            } else {
                                folderStack.top()->removeItem(selected);
                                itemRemovalCheck();
                            }
                            eatFood(player->getUnit(), itemType);
                            addMessage("You eat the " + itemType->getName() + ". " + foodTastes[itemType->getStatValue(S_TASTE)], black);
                        }
                    } else {
                        addMessage("That is not a food.", gray);
                    }
                } break;
                default: break;
            }
        }
    } else if (state == STATE_TARGET) {
        switch(stateAction) {
            case SA_FIRE: {
            int range = typeRanges[getItemType(primeFolder->getEquips()->getItem(E_RHAND)->itemType)->getType()];
            if (range) {
                Item* items = primeFolder->getBag()->getItems();
                for (int i = 0; i < primeFolder->getBag()->getNumItems(); i++) {
                    ItemType* itemType = getItemType(items[i].itemType);
                    if (itemType->getType() == range) {
                        primeFolder->getEquips()->equipItem(items[i], itemType->getType());
                        Item item = items[i];
                        if (items[i].quantityCharge > 1) { //m17
                            items[i].quantityCharge--;
                        } else {
                            primeFolder->getBag()->removeItem(i);
                            itemRemovalCheck();
                        }
                        item.quantityCharge = 1;
                        Unit* enemy = unitsInRange[stIndex];
                        addItemToPlace(enemy->x, enemy->y, player->getZone(), item);
                        addProj(player->getUnit()->x * TILE_SIZE, player->getUnit()->y * TILE_SIZE, enemy->x * TILE_SIZE, enemy->y * TILE_SIZE, 10, 0);
                        shootUnit(player->getUnit(), unitsInRange[stIndex], player->getZone());
                        primeFolder->getEquips()->removeItem(SECRET_AMMO_INDEX);
                        break;
                    }
                }
            } } break;
            default: break;
        }
        state = STATE_PLAY;
    } else if (state == STATE_SPELL) {
        state = STATE_PLAY;
        unsigned int spellI = 0;
        for (int i = 9; i >= 0; i--) {
            spellI = (spellI << 1) | circleSelect[i];
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
