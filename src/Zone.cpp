#include "Zone.h"

Location* def = new Location(0);

Zone::Zone(string nam, int w, int h, int light, bool fill): StatHolder(V_ZONE) {
    width = w;
    height = h;
    name = nam;
    filled = fill;
    if (fill) locs = new Location[width * height];
    lightness = light;

    stackIndex = -1;
    stackDepth = 0;

    static int foon = 0;
    ind = foon;
    foon++;
}

Zone::~Zone() {
    if (filled) {
        for (int i = 0; i < width * height; i++) {
            locs[i].clearStuff();
        }
        delete[] locs;
        filled = false;
    }
}

bool Zone::isFilled() {
    return filled;
}

void Zone::fill() {
    if (!filled) {
        locs = new Location[width * height];
        filled = true;
    }
}

string Zone::getName() {
    return name;
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

void Zone::tagDungeon(int index, int depth) {
    stackIndex = index;
    stackDepth = depth;
}
pair<int, int> Zone::dungeonTag() {
    return pair<int, int>(stackIndex, stackDepth);
}

int Zone::getFoon() {
    return ind;
}
