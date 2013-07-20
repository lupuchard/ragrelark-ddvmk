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

#include "Zone.h"

Location* def = new Location(0);

Zone::Zone(String nam, int w, int h, int light, bool fill): StatHolder(V_ZONE), name(nam) {
    width = w;
    height = h;
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

String Zone::getName() {
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

Location* Zone::getLocationAt(Coord l) {
    return &locs[l.index(width)];
}

Location* Zone::getLocationAt(int i) {
    return &locs[i];
}

Location* Zone::safeGetLocationAt(Coord l) {
    if (l.inBounds(width, height)) {
        return getLocationAt(l);
    }
    return def;
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
std::pair<int, int> Zone::dungeonTag() {
    return std::pair<int, int>(stackIndex, stackDepth);
}

int Zone::getFoon() {
    return ind;
}
