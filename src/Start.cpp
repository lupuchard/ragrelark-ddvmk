#include "Start.h"

const char Start::xDirs[10] = {0, -1,  0,  1, -1,  0,  1, -1,  0,  1};
const char Start::yDirs[10] = {0,  1,  1,  1,  0,  0,  0, -1, -1, -1};
//const char Start::fwubs[10] = {0,  3,  0,  0,  3,  0,  1,  2,  2,  1};

Start::Start() {
    done = false;

    gotsStructureTex = false;
    gotsMenuTex = false;
    gotsFontTex = false;
    gotsSplatterTex = false;
    gotsAttackAnimsTex = false;

    selected = 0;
    menuUp = 0;

    interval1 = 0;
    interval2 = 0;
    interval3 = 0;

    loadStatus = 0;

    shiftIsDown = false;

    messages.reserve(MAX_MESSAGES);
}

Start::~Start() {

}

void Start::prepare() {

    setFormUser(this);
    setEnvironmentManager(this);

    world = new World();
    player = new Player();

    done = !init();

    loadData(world, player);

    primeFolder = new PrimeFolder();
    Item* primeFolders = primeFolder->getItems();
    putItemFolder(primeFolders + 2, primeFolder->getGround());
    putItemFolder(primeFolders + 1, primeFolder->getEquips());
    putItemFolder(primeFolders, primeFolder->getBag());
    folderStack.push(primeFolder);

    primeFolder->getGround()->setLocation(player->getZone(), player->getUnit()->x, player->getUnit()->y);

    finishDataSetup();

    for (int i = 0; i < MAX_ZONE_SIZE; i++) {
        splatters[i] = 255;
        visibilities[i] = 0;
    }
    findAreaUnits();

    addMessage("Welcome to game great fun play.", black);
}

void Start::execute() {

    int numFrames = 0;
    Uint32 startTime = SDL_GetTicks();
    playerFieldOfView(true);
    while(!done) {
        events();
        logic();
        render();
        numFrames++;
        double fps = (numFrames / (float)(SDL_GetTicks() - startTime)) * 1000;
        if (numFrames % 1000 == 0) {
            cout << "FPS: " << fps << endl;
            numFrames = 0;
            startTime = SDL_GetTicks();
        }
    }
    cleanup();
}

int main() {
    Start start;
    start.prepare();
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

#define MESSAGE_LEN_LIMIT 90

void Start::addMessage(string message, color c) {
    pair<string, color> completeMess = pair<string, color>(message, c);
    static int forbs = 1;
    int ind = messages.size() - 1;
    if (ind > 0 && messages[ind].first.substr(0, message.size()) == message) {
        forbs++;
        messages[ind].first = message + " x" + its(forbs);
    } else {
        forbs = 1;
        color c1 = messages[ind].second;
        color c2 = messages[ind - 1].second;
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
    for (int i = 0; i < area->getNumZones(); i++) {
        Zone* zone = area->getZone(i);
        if (zone->isFilled()) {
            for (int j = 0; j < zone->getWidth(); j++) {
                for (int k = 0; k < zone->getHeight(); k++) {
                    Location* loc = zone->getLocationAt(j, k);
                    if (loc->hasUnit()) {
                        areaUnits.insert(pair<Unit*, Zone*>(loc->unit, zone));
                        loc->unit->makeHashMaps();
                    }
                }
            }
        }
    }
}

void Start::addItemToPlace(int x, int y, Zone* z, Item item) {
    z->getLocationAt(x, y)->addItem(item);
    ItemType* itemType = getItemType(item.itemType);
    int light = itemType->getStatValue(S_LIGHT);
    if (light > 0) {
        myFovCirclePerm(z, x, y, light, 1);
        if (z == player->getZone()) {
            playerFieldOfView(false);
        }
    }
}

Item Start::removeItemFromPlace(int x, int y, Zone* z, int index) {
    Item item = z->getLocationAt(x, y)->removeItem(index);
    ItemType* itemType = getItemType(item.itemType);
    int light = itemType->getStatValue(S_LIGHT);
    if (light > 0) {
        myFovCirclePerm(z, x, y, light, -1);
        if (z == player->getZone()) {
            playerFieldOfView(false);
        }
    }
    return item;
}
