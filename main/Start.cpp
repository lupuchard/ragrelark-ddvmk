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

Start::Start(): startGameButton() {
    selected = 0;
    state = STATE_MAIN_MENU;
    stateAction = SA_NONE;

    for(unsigned int i = 0; i < 4; i++) intervals[i] = 0;
    foon = 0;

    topPanel = PANEL_TOPSTART + 1;
    botPanel = PANEL_BOTTOMSTART + 1;
    notesSelected = 0;
    loadStatus = 0;
    shiftIsDown = false;

    messages.reserve(MAX_MESSAGES);

    primeFolder = NULL;
    player = NULL;

    done = prepare();

    mouse.leftDown = mouse.rightDown = mouse.leftPress = mouse.rightPress = false;
}

Start::~Start() { }

bool Start::prepare() {
    Stat::setFormUser(this);
    setEnvironmentManager(this);
    mobSpawner = new MobSpawner(this);
    setMobSpawner(mobSpawner);
    tiledLoader = new TiledLoader(this, mobSpawner);

    world = new World();

    if (!init()) return true;
    startRenderer();

    return false;
}

void Start::execute() {
    int numFrames = 0;
    //Uint32 startTime = SDL_GetTicks();
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
    if (item.getType()->isFolder()) {
        int num = item.form * 256 + item.quantityCharge;
        return folders[item.itemType][num];
    }
    return primeFolder;
}

void Start::putItemFolder(Item* item, ItemFolder* itemFolder) {
    if (item->getType()->isFolder()) {
        if (folders.find(item->itemType) != folders.end()) {
            int size = folders[item->itemType].size();
            item->quantityCharge = size / 256;
            item->form = size % 256;
        }
        folders[item->itemType].push_back(itemFolder);
    }
}

#define MESSAGE_LEN_LIMIT 78

void Start::addMessage(String message, Color c) {
    std::pair<String, Color> completeMess = std::make_pair(message, c);
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
                    }
                }
            }
        }
    }
}

void Start::addItemToPlace(Coord c, Zone* z, Item item) {
    z->getLocationAt(c)->addItem(item);
    ItemType* itemType = item.getType();
    int light = itemType->getStatValue(Stat::LIGHT);
    if (light > 0) {
        myFovCirclePerm(z, c, light, 1);
        if (z == player->getZone()) {
            playerFieldOfView(false);
        }
    }
}

Item Start::removeItemFromPlace(Coord c, Zone* z, int index) {
    Item item = z->getLocationAt(c)->removeItem(index);
    ItemType* itemType = item.getType();
    int light = itemType->getStatValue(Stat::LIGHT);
    if (light > 0) {
        myFovCirclePerm(z, c, light, -1);
        if (z == player->getZone()) {
            playerFieldOfView(false);
        }
    }
    return item;
}

Skill* Start::skll(SkillE skillIndex) {
    return Stat::getSkill(skillIndex);
}

void Start::parsePlayer(YAML::Node n, std::ostream& lerr) {
    player->setName(readYAMLStr(n, "Name", "anonymous"));
    player->setUnitProto(Unit::getMob(readYAMLStr(n, "Unit", "player"))->proto);
    player->setArea(world->getArea(readYAMLStr(n, "Area", "no area specified")));
    Zone* zone = world->getZone(readYAMLStr(n, "Zone", "no zone specified"));
    player->setZone(zone);
    player->getUnit()->pos = readYAMLCoord(n, "Loc", ORIGIN);
    zone->getLocationAt(player->getUnit()->pos)->unit = player->getUnit();
}
