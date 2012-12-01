#include "Start.h"

string foodTastes[] = {"It tastes rather bland.", "It was delicious!", "Hmm, pretty good.", "Salty.", "Yikes! It's very spicy.", "It tastes terrible."};

void Start::directionPress(int direction) {
    if (!menuUp) {
        if (direction == 5) {
            player->getUnit()->theTime += 5;
        } else {
            moveUnit(player->getUnit(), player->getZone(), direction);
            primeFolder->getGround()->setLocation(player->getZone(), player->getUnit()->x, player->getUnit()->y);
        }
    } else {
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
                if (selected == 0) {
                    selected = folderStack.top()->getNumItems() - 1;
                } else {
                    selected--;
                } break;
        }
    }
}

void Start::events() {
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: done = true; break;
            case SDL_KEYDOWN:
            if (shiftIsDown) {
                switch(event.key.keysym.sym) {
                    case SDLK_COMMA:  goTheStairs(player->getUnit(), player->getZone()); break;
                    case SDLK_PERIOD: goTheStairs(player->getUnit(), player->getZone()); break;

                    case SDLK_i: openInventory(); menuAction = MA_EXAMINE; break;
                    case SDLK_g: groundGrab(); break;
                    case SDLK_e: openEquipment(); menuAction = MA_EXAMINE;   break;
                    case SDLK_q: openBag();       menuAction = MA_EQUIP;   break;
                    case SDLK_u: openEquipment(); menuAction = MA_GRAB;    break;
                    case SDLK_c: closeDoors(); break;
                    default: break;
                }
            } else {
                switch(event.key.keysym.sym) {
                    case SDLK_7:     directionPress(7); break;
                    case SDLK_8:     directionPress(8); break;
                    case SDLK_9:     directionPress(9); break;
                    case SDLK_u:     directionPress(4); break;
                    case SDLK_i:     directionPress(5); break;
                    case SDLK_o:     directionPress(6); break;
                    case SDLK_j:     directionPress(1); break;
                    case SDLK_k:     directionPress(2); break;
                    case SDLK_l:     directionPress(3); break;
                    case SDLK_g:     directionPress(4); break;
                    case SDLK_c:     directionPress(5); break;
                    case SDLK_r:     directionPress(6); break;
                    case SDLK_h:     directionPress(1); break;
                    case SDLK_t:     directionPress(2); break;
                    case SDLK_n:     directionPress(3); break;
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

                    case SDLK_s:      openInventory(); menuAction = MA_EXAMINE; break;
                    case SDLK_PERIOD: openInventory(); menuAction = MA_EXAMINE; break;
                    case SDLK_x:      openBag();       menuAction = MA_EXAMINE; break;
                    case SDLK_w:      openGround();    menuAction = MA_GRAB;    break;
                    case SDLK_COMMA:  groundGrab();    menuAction = MA_GRAB;    break;
                    case SDLK_d:      openBag();       menuAction = MA_DROP;    break;
                    case SDLK_e:      openBag();       menuAction = MA_EAT;     break;

                    case SDLK_LSHIFT: shiftIsDown = true; break;
                    case SDLK_RSHIFT: shiftIsDown = true; break;
                    case SDLK_ESCAPE: menuUp = 0; selected = 0; break;
                    case SDLK_SPACE:  enterCommand(); /*createEffect(P_DARKDUST, 100, 100);*/ addProj(200, 200, player->getUnit()->x * 32, player->getUnit()->y * 32, 60, 0); break;
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
    menuUp = 1;
}

void Start::openBag() {
    if (primeFolder->getBag()->getNumItems() > 0) {
        while(folderStack.top() != primeFolder) {
            folderStack.pop();
        }
        folderStack.push(primeFolder->getBag());
        menuUp = 1;
    }
}

void Start::openEquipment() {
    while(folderStack.top() != primeFolder) {
        folderStack.pop();
    }
    folderStack.push(primeFolder->getEquips());
    menuUp = 1;
}

void Start::openGround() {
    if (primeFolder->getGround()->getNumItems() > 0) {
        while(folderStack.top() != primeFolder) {
            folderStack.pop();
        }
        folderStack.push(primeFolder->getGround());
        menuUp = 1;
    }
}

void Start::action(int action, Unit* unit, int value1, int value2, int value3) {
    switch(action) {
        case A_MOVEDIR:
            rMoveDir(unit, value1, value2, value3);
            break;
        default: break;
    }
}

bool Start::equipItem(Item item) {
    ItemType* itemType = getItemType(item.itemType);
    if (typeSlots[itemType->getType()] != E_NONE) {
        Item oldItem = primeFolder->getEquips()->equipItem(item, itemType->getType());
        int oldItemTypeType = getItemType(oldItem.itemType)->getType();
        if (!(oldItemTypeType == 0 || oldItemTypeType == I_SLOT)) {
            if (!primeFolder->getBag()->addItem(&oldItem)) {
                primeFolder->getGround()->addItem(&oldItem);
            }
        }
        if (typeSlots[itemType->getType()] == E_BAG) {
            primeFolder->getBag()->setCapacity(itemType->getStatValue(S_CAPACITY));
        }
        vector<Stat*> itemAfflictions = getItemAfflictions();
        for (vector<Stat*>::iterator i = itemAfflictions.begin(); i != itemAfflictions.end(); i++) {
            Stat* theStat = (Stat*)*i;
            player->getUnit()->needToUpdate(theStat->getIndex(), theStat->isItFloat());
        }
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
    if (menuUp) {
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
                    itemRemovalCheck();
                } break;
                case MA_DROP: {
                    Item item = folderStack.top()->removeItem(selected);
                    int itemTypeType = getItemType(item.itemType)->getType();
                    if (!(itemTypeType == 0 || itemTypeType == I_SLOT)) {
                        //primeFolder->getGround()->addItem(&item);
                        addItemToPlace(player->getUnit()->x, player->getUnit()->y, player->getZone(), item);
                    }
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
                            if (item->quantityCharge > 1) {
                                item->quantityCharge--;
                            } else {
                                folderStack.top()->removeItem(selected);
                            }
                            eatFood(player->getUnit(), itemType);
                            itemRemovalCheck();
                            addMessage("You eat the " + itemType->getName() + ". " + foodTastes[itemType->getStatValue(S_TASTE)], black);
                        }
                    } else {
                        addMessage("That is not a food.", gray);
                    }
                } break;
                default: break;
            }
        }
    }
}

void Start::backCommand() {
    if (menuUp) {
        if (folderStack.top() != primeFolder) {
            folderStack.pop();
        } else {
            menuUp = 0;
        }
        selected = 0;
    }
}
