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

#include "DungeonStack.h"

Generator generator;
MobSpawner* mobSpawner;

void setMobSpawner(MobSpawner* mobSp) {
    mobSpawner = mobSp;
}

DungeonStack::DungeonStack(String n, int depthS, GenType genTypeS): name(n) {
    zones = new Zone*[depthS];
    depth = depthS;
    depthReached = 0;
    genType = genTypeS;
    width = 64;
    height = 64;
    widthChange = 0.f;
    heightChange = 0.f;

    light = 6;
    lightChange = 0.f;
    level = 1;
    levelChange = 0.f;
    numStairs = -1;
    stairsChange = 0.f;
    enemyPerSquare = .002f;
    eDensityChange = .0f;
    itemsPerSquare = .005f;
    iDensityChange = .0f;

    skeleton = NULL;
    tileset = NULL;
    stairLocs = NULL;
}

DungeonStack::~DungeonStack() {
    if (skeleton) {
        if (depthReached != depth) {
            delete[] skeleton;
        }
        for (int i = 0; i < depth; i++) {
            delete zones[i];
        }
        for (int i = 0; i < std::min(depthReached, depth - 1); i++) {
            delete[] stairLocs[i];
        }
        delete[] stairLocs;
    }
    delete[] zones;
}

void DungeonStack::setLight(int lightN, float change) {
    light = lightN;
    lightChange = change;
}

void DungeonStack::setDifficulty(int diff, float change) {
    level = diff;
    levelChange = change;
}

void DungeonStack::addEnvironment(short enviro) {
    environments.push_back(enviro);
}

void DungeonStack::setNumStairs(int stairs, float change) {
    numStairs = stairs;
    stairsChange = change;
}

void DungeonStack::setDimensions(int w, int h, float wChange, float hChange) {
    width = w;
    height = h;
    widthChange = wChange;
    heightChange = hChange;
}

void DungeonStack::setEnemyDensity(float enePerSquare, float change) {
    enemyPerSquare = enePerSquare;
    eDensityChange = change;
}

void DungeonStack::setItemDensity(float itePerSquare, float change) {
    itemsPerSquare = itePerSquare;
    iDensityChange = change;
}

void DungeonStack::setTileset(TileSet* ts) {
    tileset = ts;
}

Zone* DungeonStack::getZone(int d) {
    return zones[d];
}

int DungeonStack::getDepth() {
    return depth;
}

Coord DungeonStack::addEntryStairs() {
    std::vector<Coord> possibleLocs;
    for (int j = 0; j < zones[0]->getWidth(); j++) {
        for (int k = 0; k < zones[0]->getHeight(); k++) {
            int skel = skeleton[j + k * zones[0]->getWidth()];
            if (skel == SKEL_FLOOR) possibleLocs.push_back(Coord(j, k));
        }
    }
    Coord foon = possibleLocs[rand() % possibleLocs.size()];
    if (depthReached) {
        zones[0]->getLocationAt(foon)->structure = S_STAIRUP;
    } else {
        skeleton[foon.index(zones[0]->getWidth())] = SKEL_STAIRS;
    }
    return foon;
}

int DungeonStack::getNumStairs(int floor) {
    return numStairs + (int)(stairsChange * floor);
}

Coord DungeonStack::getStairLoc(int floor, int index) {
    return stairLocs[floor][index];
}

void DungeonStack::createZones() {
    stairLocs = new Coord*[depth - 1];
    for (int i = 0; i < depth; i++) {
        int currLight = std::max(light + (int)(lightChange * i), 0);
        zones[i] = new Zone(name + " " + its(i), width + (int)(widthChange * i), height + (int)(heightChange * i), currLight, false);
    }
    skeleton = generator.genSkeleton(width, height, genType);
}
void DungeonStack::genLevel(int floor, std::vector<std::pair<Unit*, Zone*> >* unitsAdded) {
    for (int i = depthReached; i < floor; i++) {
        int wid = zones[i]->getWidth();
        int hei = zones[i]->getHeight();

        std::vector<Coord> possibleLocs;
        for (int j = 0; j < wid; j++) {
            for (int k = 0; k < hei; k++) {
                if (!skeleton[j + k * wid]) possibleLocs.push_back(Coord(j, k));
            }
        }
        generator.fillSkeleton(skeleton, zones[i], tileset);

        if (i < depth - 1) {
            unsigned char* newSkeleton;
            int nzWid = zones[i + 1]->getWidth();
            int nzHei = zones[i + 1]->getHeight();
            newSkeleton = generator.genSkeleton(nzWid, nzHei, genType);
            std::vector<Coord> sharedLocs;
            int w = std::min(wid, nzWid);
            int h = std::min(hei, nzHei);
            for (int j = 0; j < w; j++) {
                for (int k = 0; k < h; k++) {
                    if (!skeleton[j + k * wid] && !newSkeleton[j + k * nzWid]) sharedLocs.push_back(Coord(j, k));
                }
            }
            int currNumStairs = numStairs + (int)(i * stairsChange);
            stairLocs[i] = new Coord[currNumStairs];
            for (int j = 0; j < currNumStairs; j++) {
                if (!sharedLocs.empty()) {
                    int v = rand() % sharedLocs.size();
                    Coord coords = sharedLocs[v];
                    sharedLocs.erase(sharedLocs.begin() + v);
                    newSkeleton[coords.index(nzWid)] = SKEL_STAIRS;
                    zones[i]->getLocationAt(coords)->structure = S_STAIRDOWN;
                    stairLocs[i][j] = coords;
                } else {
                    std::cout << "fONONONONONONNO" << std::endl;
                    stairLocs[i][j] = Coord(-1, -1);
                }
            }
            delete[] skeleton;
            skeleton = newSkeleton;
        }

        int currLevel = level + i * levelChange;
        int numEnemies = (int)(wid * hei * (enemyPerSquare + eDensityChange * i));
        int numItems = (int)(wid * hei * (itemsPerSquare + iDensityChange * i));
        mobSpawner->createEncounters(zones[i], environments.size(), &environments[0], currLevel, numEnemies, possibleLocs, unitsAdded);
        mobSpawner->createItems(zones[i], environments.size(), &environments[0], currLevel, numItems, possibleLocs);
        //mobSpawner->overgrowth(zones[i], genType, ORIGIN, Coord(wid, hei));
    }
    depthReached = floor;
}

void DungeonStack::save(std::ostream& saveData) {
    outSht(depth, saveData);
    outSht(depthReached, saveData);
    outSht(genType, saveData);

    outSht(environments.size(), saveData);
    for (unsigned int i = 0; i < environments.size(); i++) {
        outSht(environments[i], saveData);
    }

    outStr(name, saveData);
    outStr(tileset->name, saveData);
    outSht(width , saveData); outFlt(widthChange , saveData);
    outSht(height, saveData); outFlt(heightChange, saveData);
    outSht(light , saveData); outFlt(lightChange , saveData);
    outSht(level , saveData); outFlt(levelChange , saveData);
    outFlt(enemyPerSquare, saveData); outFlt(eDensityChange, saveData);
    outFlt(itemsPerSquare, saveData); outFlt(iDensityChange, saveData);
    outSht(numStairs, saveData); outFlt(stairsChange, saveData);

    for (int i = 0; i < depth; i++) {
        zones[i]->save(saveData);
    }
    if (depthReached + 1 < depth) {
        Zone* z = zones[depthReached + 1];
        for (int j = 0; j < z->getWidth() * z->getHeight(); j++) {
            saveData.put(skeleton[j]);
        }
    }
    for (int i = 0; i < std::min(depthReached, depth - 1); i++) {
        int currNumStairs = numStairs + (int)(i * stairsChange);
        for (int j = 0; j < currNumStairs; j++) {
            outPair(stairLocs[i][j], saveData);
        }
    }
}

DungeonStack::DungeonStack(std::istream& saveData) {
    depth = inSht(saveData);
    depthReached = inSht(saveData);
    genType = (GenType)inSht(saveData);

    unsigned int numEnvironments = inSht(saveData);;
    environments.resize(numEnvironments);
    for (unsigned int i = 0; i < numEnvironments; i++) {
        environments[i] = inSht(saveData);
    }

    name = inStr(saveData);
    String tilesetName = inStr(saveData);
    tileset = Tile::getSet(tilesetName);

    width  = inSht(saveData); widthChange  = inFlt(saveData);
    height = inSht(saveData); heightChange = inFlt(saveData);
    light  = inSht(saveData); lightChange  = inFlt(saveData);
    level  = inSht(saveData); levelChange  = inFlt(saveData);
    enemyPerSquare = inFlt(saveData); eDensityChange = inFlt(saveData);
    itemsPerSquare = inFlt(saveData); iDensityChange = inFlt(saveData);
    numStairs = inSht(saveData); stairsChange = inFlt(saveData);

    zones = new Zone*[depth];
    for (int i = 0; i < depth; i++) {
        zones[i] = new Zone(saveData);
    }
    if (depthReached + 1 < depth) {
        Zone* z = zones[depthReached + 1];
        skeleton = new unsigned char[z->getWidth() * z->getHeight()];
        for (int j = 0; j < z->getWidth() * z->getHeight(); j++) {
            skeleton[j] = saveData.get();
        }
    } else skeleton = NULL;

    stairLocs = new Coord*[depth - 1];
    for (int i = 0; i < std::min(depthReached, depth - 1); i++) {
        int currNumStairs = numStairs + (int)(i * stairsChange);
        stairLocs[i] = new Coord[currNumStairs];
        for (int j = 0; j < currNumStairs; j++) {
            stairLocs[i][j] = inPair(saveData);
        }
    }
}
