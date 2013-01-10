#include "Zone.h"

Location* def = new Location(0);

Zone::Zone(int w, int h, int numDiffTiles, int light, int fill, int mType): StatHolder(V_ZONE) {
    hasModifications = false;
    width = w;
    height = h;
    if (numDiffTiles) {
        tiles = new Tile*[numDiffTiles];
        //ownTileset = true;
    }
    if (fill < 0) {
        filled = fill;
    } else if (fill > 0) {
        locs = new Location[width * height];
        filled = 1;
    } else {
        filled = 0;
    }
    numTiles = 0;
    lightness = light;
    mobType = mType;
    mobSpawnLevel = 0;
}

Zone::~Zone() {
    if (numTiles < 0) {
        delete[] tiles;
    }
    if (filled) {
        delete[] locs;
    }
}

void Zone::setTileset(int num, Tile** tileset) {
    numTiles = num;
    tiles = tileset;
}

int Zone::isFilled() {
    return filled > 0;
}

GenType Zone::getGenType() {
    return (GenType)-filled;
}

int Zone::getMobType() {
    return mobType;
}

void Zone::fill() {
    if (filled <= 0) {
        locs = new Location[width * height];
        filled = 1;
    }
}

void Zone::becomeGenned() {
    filled = 2;
    if (modifications) {
        for (unsigned int i = 0; i < modifications->size(); i++) {
            modification m = (*modifications)[i];
            switch(m.type) {
                case '$': getLocationAt(m.x, m.y)->addItem(Item(m.value)); break;
                //case '@': TODO spawn unit
                case '^': getLocationAt(m.x, m.y)->height = m.value; break;
                case '<': getLocationAt(m.x, m.y)->structure = m.value; break;
                case '.': getLocationAt(m.x, m.y)->tile = m.value; break;
                case '0': break;
                default: break;
            }
        }
        delete modifications;
    }
}

void Zone::addModification(int x, int y, char type, int value) {
    if (!hasModifications) {
        modifications = new vector<modification>;
        hasModifications = true;
    }
    modification m;
    m.x = x;
    m.y = y;
    m.type = type;
    m.value = value;
    modifications->push_back(m);
}

int Zone::getNumReserved() {
    return modifications->size();
}

pair<int, int> Zone::getRes(int i) {
    modification m = (*modifications)[i];
    return pair<int, int>(m.x, m.y);
}

int Zone::getNumTiles() {
    return numTiles;
}

int Zone::getWidth() {
    return width;
}

int Zone::getHeight() {
    return height;
}

unsigned char Zone::getLightness() {
    return lightness;
}

unsigned char Zone::getMobSpawnLevel() {
    return mobSpawnLevel;
}

void Zone::setMobSpawnLevel(unsigned char v) {
    mobSpawnLevel = v;
}

Tile* Zone::getTileAt(int x, int y) {
    return tiles[locs[x + y * width].tile];
}

Tile* Zone::safeGetTileAt(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= width) {
        return tiles[0];
    }
    return getTileAt(x, y);
}

Location* Zone::getLocationAt(int x, int y) {
    return &locs[x + y * width];
}

Location* Zone::getLocationAt(int i) {
    return &locs[i];
}

Location* Zone::safeGetLocationAt(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= width) {
        return def;
    }
    return getLocationAt(x, y);
}

int Zone::addTile(Tile* t) {
    tiles[(int)numTiles] = t;
    return numTiles++;
}

void Zone::fillTiles(int* tiles) {
    int totes = width * height;
    for (int i = 0; i < totes; i++) {
        locs[i].tile = tiles[i];
    }
}

void Zone::fillHeights(int* tiles) {
    int totes = width * height;
    for (int i = 0; i < totes; i++) {
        locs[i].height = tiles[i];
    }
}

void Zone::fillStructs(int* structs) {
    int totes = width * height;
    for (int i = 0; i < totes; i++) {
        locs[i].structure = structs[i];
    }
}
