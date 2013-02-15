#include "DungeonStack.h"

Generator generator;
MobSpawner* mobSpawner;

void setMobSpawner(MobSpawner* mobSp) {
    mobSpawner = mobSp;
}

DungeonStack::DungeonStack(string n, int depthS, GenType genTypeS) {
    zones = new Zone*[depthS];
    name = n;
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
}

DungeonStack::~DungeonStack() {
    if (depthReached != depth) {
        delete[] skeleton;
    }
    for (int i = 0; i < depth; i++) {
        delete zones[i];
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

void DungeonStack::setEnvironments(int numEnvironments, short* enviros) {
    environments.insert(environments.begin(), enviros, enviros + numEnvironments);
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

void DungeonStack::setTileset(Tile** ts) {
    tileset = ts;
}

Zone* DungeonStack::getZone(int d) {
    return zones[d];
}

int DungeonStack::getDepth() {
    return depth;
}

pair<short, short> DungeonStack::addEntryStairs() {
    vector<pair<int, int> > possibleLocs;
    for (int j = 0; j < zones[0]->getWidth(); j++) {
        for (int k = 0; k < zones[0]->getHeight(); k++) {
            int skel = skeleton[j + k * zones[0]->getWidth()];
            if (skel == SKEL_FLOOR) possibleLocs.push_back(pair<short, short>(j, k));
        }
    }
    pair<short, short> foon = possibleLocs[rand() % possibleLocs.size()];
    if (depthReached) {
        zones[0]->getLocationAt(foon.first, foon.second)->structure = S_STAIRUP;
    } else {
        skeleton[foon.first + foon.second * zones[0]->getWidth()] = SKEL_STAIRS;
    }
    return foon;
}

int DungeonStack::getNumStairs(int floor) {
    return numStairs + (int)(stairsChange * floor);
}

pair<short, short> DungeonStack::getStairLoc(int floor, int index) {
    return pair<short, short>(stairXs[floor][index], stairYs[floor][index]);
}

void DungeonStack::createZones() {
    stairXs = new short*[depth - 1];
    stairYs = new short*[depth - 1];
    for (int i = 0; i < depth; i++) {
        int currLight = max(light + (int)(lightChange * i), 0);
        zones[i] = new Zone(name + " " + its(i), width + (int)(widthChange * i), height + (int)(heightChange * i), currLight, false);
    }
    skeleton = generator.genSkeleton(width, height, genType);
}
void DungeonStack::genLevel(int floor, vector<pair<Unit*, Zone*> >* unitsAdded) {
    for (int i = depthReached; i < floor; i++) {
        int wid = zones[i]->getWidth();
        int hei = zones[i]->getHeight();

        vector<pair<int, int> > possibleLocs;
        for (int j = 0; j < wid; j++) {
            for (int k = 0; k < hei; k++) {
                if (!skeleton[j + k * wid]) possibleLocs.push_back(pair<int, int>(j, k));
            }
        }
        generator.fillSkeleton(skeleton, zones[i], tileset);

        if (i < depth - 1) {
            unsigned char* newSkeleton;
            int nzWid = zones[i + 1]->getWidth();
            int nzHei = zones[i + 1]->getHeight();
            newSkeleton = generator.genSkeleton(nzWid, nzHei, genType);
            vector<pair<int, int> > sharedLocs;
            int w = min(wid, nzWid);
            int h = min(hei, nzHei);
            for (int j = 0; j < w; j++) {
                for (int k = 0; k < h; k++) {
                    if (!skeleton[j + k * wid] && !newSkeleton[j + k * nzWid]) sharedLocs.push_back(pair<int, int>(j, k));
                }
            }
            int currNumStairs = numStairs + (int)(i * stairsChange);
            stairXs[i] = new short[currNumStairs];
            stairYs[i] = new short[currNumStairs];
            for (int j = 0; j < currNumStairs; j++) {
                if (!sharedLocs.empty()) {
                    int v = rand() % sharedLocs.size();
                    pair<int, int> coords = sharedLocs[v];
                    sharedLocs.erase(sharedLocs.begin() + v);
                    newSkeleton[coords.first + coords.second * nzWid] = SKEL_STAIRS;
                    zones[i]->getLocationAt(coords.first, coords.second)->structure = S_STAIRDOWN;
                    stairXs[i][j] = coords.first;
                    stairYs[i][j] = coords.second;
                } else {
                    cout << "fONONONONONONNO" << endl;
                    stairXs[i][j] = -1;
                    stairYs[i][j] = -1;
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
        mobSpawner->overgrowth(zones[i], genType, 0, 0, wid, hei);
    }
    depthReached = floor;
}
