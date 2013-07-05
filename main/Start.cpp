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

Start::Start() {
    done = false;

    gotsStructureTex = false;
    gotsMenuTex = false;
    gotsFontTex = false;
    gotsSplatterTex = false;
    gotsAttackAnimsTex = false;

    selected = 0;
    state = STATE_PLAY;
    stateAction = SA_NONE;

    interval0 = 0;
    interval1 = 0;
    interval2 = 0;
    interval3 = 0;
    foon = 0;

    topPanel = PANEL_TOPSTART + 1;
    botPanel = PANEL_BOTTOMSTART + 1;
    notesSelected = 0;

    loadStatus = 0;

    shiftIsDown = false;

    messages.reserve(MAX_MESSAGES);

    prepare();
}

Start::~Start() {

}

void Start::prepare() {

    setFormUser(this);
    setEnvironmentManager(this);
    setEnvironmentManager(this);
    mobSpawner = new MobSpawner(this);
    setMobSpawner(mobSpawner);

    world = new World();
    primeFolder = new PrimeFolder();
    player = new Player(primeFolder);

    done = !init();

    loadData(world, player);

    Item* primeFolders = primeFolder->getItems();
    putItemFolder(primeFolders + 2, primeFolder->getGround());
    putItemFolder(primeFolders + 1, primeFolder->getEquips());
    putItemFolder(primeFolders, primeFolder->getBag());
    folderStack.push(primeFolder);

    primeFolder->getGround()->setLocation(player->getZone(), player->getUnit()->pos);

    finishDataSetup();

    for (int i = 0; i < MAX_ZONE_SIZE; i++) {
        visibilities[i] = 0;
    }
    findAreaUnits();

    addMessage("Welcome to game great fun play.", BLACK);
}

void Start::execute() {
    int numFrames = 0;
    //Uint32 startTime = SDL_GetTicks();
    playerFieldOfView(true);
    while(!done) {
        events();
        logic();
        render();
        numFrames++;
        //double fps = (numFrames / (float)(SDL_GetTicks() - startTime)) * 1000;
        if (numFrames % 1000 == 0) {
            //cout << "FPS: " << fps << endl;
            numFrames = 0;
            //startTime = SDL_GetTicks();
        }
    }
    cleanup();
}

int main() {
    Start start;
    start.execute();

    return 0;
}

ItemFolder* Start::getItemFolder(Item item) {
    if (getItemType(item.itemType)->getType() >= 10 && getItemType(item.itemType)->getType() <= 14) {
        int num = item.form * 256 + item.quantityCharge;
        return folders[item.itemType][num];
    }
    return primeFolder;
}

void Start::createItemFolder(Item* item) {
    int itemTypeType = getItemType(item->itemType)->getType();
    if (itemTypeType >= 10 && itemTypeType <= 14) {
        if (folders.find(item->itemType) != folders.end()) {
            int size = folders[item->itemType].size();
            item->quantityCharge = size / 256;
            item->form = size % 256;
        }
        switch(itemTypeType) {
            case 10: break;
            case 11: folders[item->itemType].push_back(new GroundFolder()); break;
            case 12: folders[item->itemType].push_back(new EquipmentFolder()); break;
            case 13: folders[item->itemType].push_back(new BagFolder(10)); break;
            case 14: folders[item->itemType].push_back(new PrimeFolder()); break;
            default: break;
        }
    }
}

void Start::putItemFolder(Item* item, ItemFolder* itemFolder) {
    int itemTypeType = getItemType(item->itemType)->getType();
    if (itemTypeType >= 10 && itemTypeType <= 14) {
        if (folders.find(item->itemType) != folders.end()) {
            int size = folders[item->itemType].size();
            item->quantityCharge = size / 256;
            item->form = size % 256;
        }
        folders[item->itemType].push_back(itemFolder);
    }
}

#define MESSAGE_LEN_LIMIT 78

void Start::addMessage(std::string message, Color c) {
    std::pair<std::string, Color> completeMess = std::make_pair(message, c);
    static int forbs = 1;
    int ind = messages.size() - 1;
    if (ind > 0 && messages[ind].first.substr(0, message.size()) == message) {
        forbs++;
        messages[ind].first = message + " x" + its(forbs);
    } else {
        forbs = 1;
        Color c1, c2;
        if (messages.size() >= 2) {
            c1 = messages[ind].second;
            c2 = messages[ind - 1].second;
        }
        if (messages.size() >= 2 && c1.red == c2.red && c1.blue == c2.blue && c1.green == c2.green && messages[ind].first.size() + messages[ind - 1].first.size() < MESSAGE_LEN_LIMIT) {
            messages[ind - 1].first = messages[ind - 1].first + " " + messages[ind].first;
            messages[ind] = completeMess;
        } else {
            messages.push_back(completeMess);
            if (messages.size() > MAX_MESSAGES) {
                messages.erase(messages.begin());
            }
        }
    }
}

void Start::findAreaUnits() {
    areaUnits.clear();
    Area* area = player->getArea();
    for (unsigned int i = 0; i < area->getNumZones(); i++) {
        Zone* zone = area->getZone(i);
        if (zone->isFilled()) {
            for (int k = 0; k < zone->getHeight(); k++) {
                for (int j = 0; j < zone->getWidth(); j++) {
                    Location* loc = zone->getLocationAt(Coord(j, k));
                    if (loc->hasUnit()) {
                        areaUnits.insert(std::pair<Unit*, Zone*>(loc->unit, zone));
                        loc->unit->makeHashMaps();
                    }
                }
            }
        }
    }
}

void Start::addItemToPlace(Coord c, Zone* z, Item item) {
    z->getLocationAt(c)->addItem(item);
    ItemType* itemType = getItemType(item.itemType);
    int light = itemType->getStatValue(S_LIGHT);
    if (light > 0) {
        myFovCirclePerm(z, c, light, 1);
        if (z == player->getZone()) {
            playerFieldOfView(false);
        }
    }
}

Item Start::removeItemFromPlace(Coord c, Zone* z, int index) {
    Item item = z->getLocationAt(c)->removeItem(index);
    ItemType* itemType = getItemType(item.itemType);
    int light = itemType->getStatValue(S_LIGHT);
    if (light > 0) {
        myFovCirclePerm(z, c, light, -1);
        if (z == player->getZone()) {
            playerFieldOfView(false);
        }
    }
    return item;
}
