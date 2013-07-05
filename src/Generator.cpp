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

#include "Generator.h"

void placeWall(Location* loc, Tile** tiles, bool alt) {
    int foo = rand() % 20;
    int k = alt * 8;
    if (foo == 0) {
        loc->tile = tiles[7 + k]->getIndex();
    } else if (foo < 4) {
        loc->tile = tiles[6 + k]->getIndex();
    } else if (foo < 8) {
        loc->tile = tiles[5 + k]->getIndex();
    } else {
        loc->tile = tiles[4 + k]->getIndex();
    }
    loc->height = MAX_HEIGHT;
}
void placeFloor(Location* loc, Tile** tiles, bool alt) {
    int foo = rand() % 20;
    int k = alt * 8;
    if (foo == 0) {
        loc->tile = tiles[3 + k]->getIndex();
    } else if (foo < 4) {
        loc->tile = tiles[2 + k]->getIndex();
    } else if (foo < 8) {
        loc->tile = tiles[1 + k]->getIndex();
    } else {
        loc->tile = tiles[0 + k]->getIndex();
    }
    loc->height = MAX_HEIGHT / 2;
}

void Generator::makeConnection(unsigned char* skeleton, ZoneNode* n1, ZoneNode* n2, int type) {
    if (n1->connections.find(n2) != n1->connections.end()) return; //if a connection already exists dont do anything
    if (!type) type = rand() % 2 + 1;
    if (type == 2) {
        ZoneNode* temp = n1;
        n1 = n2;
        n2 = temp;
    }
    if (n1->x < n2->x) {
        for (int i = n1->x; i <= n2->x; i++) skeleton[i + n2->y * toteWidth] = SKEL_FLOOR;
    } else if (n1->x > n2->x) {
        for (int i = n2->x; i <= n1->x; i++) skeleton[i + n2->y * toteWidth] = SKEL_FLOOR;
    }
    if (n1->y < n2->y) {
        for (int i = n1->y; i <= n2->y; i++) skeleton[n1->x + i * toteHeight] = SKEL_FLOOR;
    } else if (n1->y > n2->y) {
        for (int i = n2->y; i <= n1->y; i++) skeleton[n1->x + i * toteHeight] = SKEL_FLOOR;
    }
    n1->connections.insert(n2);
    n2->connections.insert(n1);
}

std::set<ZoneNode*> accessable;

void depthSearch(ZoneNode* start) {
    start->done = true;
    accessable.insert(start);

    int j = 0;
    for (std::set<ZoneNode*>::iterator i = start->connections.begin(); i != start->connections.end(); ++i) {
        ZoneNode* n = (ZoneNode*)(*i);
        if (!n->done) {
            depthSearch(n);
        }
        j++;
        if (j > 1000) {
            std::cout << "MEGALOOP depthSearch" << std::endl;
        }
    }
}

Generator::Generator(): toteWidth(0), toteHeight(0) {}
Generator::~Generator() {}

void Generator::fillSkeleton(unsigned char* skeleton, Zone* zone, Tile** tiles) {
    zone->fill();
    for (int x = 0; x < zone->getWidth(); x++) {
        for (int y = 0; y < zone->getHeight(); y++) {
            Location* loc = zone->getLocationAt(Coord(x, y));
            if (skeleton[x + y * zone->getWidth()] == SKEL_WALL) {
                placeWall(loc, tiles, false);
                loc->structure = S_NONE;
            } else if (skeleton[x + y * zone->getWidth()] == SKEL_FLOOR) {
                placeFloor(loc, tiles, false);
                loc->structure = S_NONE;
            } else if (skeleton[x + y * zone->getWidth()] == SKEL_DOOR) {
                placeFloor(loc, tiles, false);
                loc->tile = tiles[4]->getIndex();
                int r = rand() % 100;
                if (r == 50) loc->structure = S_WOODDOOR_BROKE;
                else if (r == 51) loc->structure = S_NORMDOOR;
                else if (r > 95) loc->structure = S_STONEDOOR;
                else if (r < 35) loc->structure = S_HIDDENDOOR;
                else loc->structure = S_WOODDOOR;
            } else if (skeleton[x + y * zone->getWidth()] == SKEL_STAIRS) {
                placeFloor(loc, tiles, false);
                loc->structure = S_STAIRUP;
            } else if (skeleton[x + y * zone->getWidth()] == SKEL_ALT) {
                loc->structure = S_NONE;
            }
        }
    }
    for (int x = 1; x < zone->getWidth() - 1; x++) {
        for (int y = 1; y < zone->getHeight() - 1; y++) {
            if (skeleton[x + y * zone->getWidth()] == SKEL_ALT) {
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        int num = x + i + (y + j) * zone->getWidth();
                        Location* loc = zone->getLocationAt(Coord(x + i, y + j));
                        if (skeleton[num] == SKEL_WALL || skeleton[num] == SKEL_DOOR) {
                            placeWall(loc, tiles, true);
                        } else if (skeleton[num] == SKEL_DOOR) {
                            placeFloor(loc, tiles, false);
                            loc->tile = tiles[12]->getIndex();
                        } else {
                            placeFloor(loc, tiles, true);
                        }
                    }
                }
            }
        }
    }
    int wid = rand() % 5 + 5;
    int hei = rand() % 5 + 5;
    int sx = rand() % (zone->getWidth() - wid - 1) + 1;
    int sy = rand() % (zone->getHeight() - hei - 1) + 1;
    for (int x = sx; x < sx + wid; x++) {
        for (int y = sy; y < sy + hei; y++) {
            Location* loc = zone->getLocationAt(Coord(x, y));
            if (loc->height == MAX_HEIGHT) {
                loc->tile = tiles[16]->getIndex();
                loc->height = MAX_HEIGHT / 4;
                for (int i = -1; i <= 1; i++) {
                    for (int j = -1; j <= 1; j++) {
                        Location* loc2 = zone->getLocationAt(Coord(x + i, y + j));
                        if (loc2->structure == S_WOODDOOR) {
                            loc2->structure = S_NONE;
                            placeFloor(loc2, tiles, false);
                        }
                    }
                }
            }
        }
    }
}

unsigned char* Generator::genSkeleton(int width, int height, GenType genType) {
    toteWidth = width;
    toteHeight = height;

    int totes = width * height;
    unsigned char* skeleton = new unsigned char[width * height];
    for (int i = 0; i < totes; i++) {
        skeleton[i] = SKEL_WALL;
    }

    int uncomminity = 0;
    switch(genType) {
        case GEN_MYDUN: genSkeletonCrazy(skeleton, 0, 0, width, height); uncomminity = 5; break;
        case GEN_BSP: genSkeletonBsptree(skeleton, 0, 0, width, height); uncomminity = 2; break;
        case GEN_DUNGEON: {
            if (rand() % 2) {
                 genSkeletonCrazy(skeleton, 0, 0, width, height);
                 uncomminity = 5;
            } else {
                genSkeletonBsptree(skeleton, 0, 0, width, height);
                uncomminity = 2;
        } } break;
        default: break;
    }

    //doors!
    if (uncomminity) {
        for (int i = 1; i < width - 1; i++) {
            for (int j = 1; j < height - 1; j++) {
                if (skeleton[i + j * width] != SKEL_WALL) {
                    unsigned char nLoc = skeleton[i + (j - 1) * width]; unsigned char sLoc = skeleton[i + (j + 1) * width];
                    unsigned char eLoc =   skeleton[i + 1 + j * width]; unsigned char wLoc =   skeleton[i - 1 + j * width];
                    if (nLoc == SKEL_WALL && sLoc == SKEL_WALL) {
                        if (eLoc == SKEL_WALL || wLoc == SKEL_WALL || wLoc == SKEL_DOOR) continue;
                    } else if (eLoc == SKEL_WALL && wLoc == SKEL_WALL) {
                        if (nLoc == SKEL_WALL || sLoc == SKEL_WALL || nLoc == SKEL_DOOR) continue;
                    } else continue;
                    unsigned char seLoc = skeleton[i + 1 + (j + 1) * width]; unsigned char neLoc = skeleton[i + 1 + (j - 1) * width];
                    unsigned char swLoc = skeleton[i - 1 + (j + 1) * width]; unsigned char nwLoc = skeleton[i - 1 + (j - 1) * width];
                    if (seLoc == SKEL_WALL && neLoc == SKEL_WALL && swLoc == SKEL_WALL && nwLoc == SKEL_WALL) continue;
                    if (!(rand() % uncomminity)) {
                        skeleton[i + j * width] = SKEL_DOOR;
                    }
                }
            }
        }
    }
    return skeleton;
}

void Generator::genSkeletonCrazy(unsigned char* skeleton, int x, int y, int wid, int hei) {
    std::vector<ZoneNode> nodes;
    int numRooms = wid * hei / (rand() % 40 + 60);

    for (int k = 0; k < numRooms; k++) {
        int sizeX = rand() % 4 + 2;
        int sizeY = rand() % 4 + 2;
        int xLoc = rand() % (wid - 8) + 4 + x;
        int yLoc = rand() % (hei - 8) + 4 + y;
        int x1 = xLoc - sizeX / 2;
        int y1 = yLoc - sizeY / 2;
        int x2 = xLoc + sizeX / 2;
        int y2 = yLoc + sizeY / 2;
        if (sizeX % 2) x2++;
        if (sizeY % 2) y2++;
        bool alt = !(rand() % 100);
        for (int i = x1; i <= x2; i++) {
            for (int j = y1; j <= y2; j++) {
                if (alt) skeleton[i + j * toteWidth] = SKEL_ALT;
                else skeleton[i + j * toteWidth] = SKEL_FLOOR;
            }
        }
        ZoneNode n;
        n.x = xLoc;
        n.y = yLoc;
        nodes.push_back(n);
    }

    for (int i = 0; i < numRooms; i++) {
        ZoneNode* n = &nodes[rand() % nodes.size()];
        int minDist = 1000000;
        ZoneNode* closest = NULL;
        for (unsigned int j = 0; j < nodes.size(); j++) {
            ZoneNode* thisN = &nodes[j];
            int cheapDist = fabs(thisN->x - n->x) + fabs(thisN->y - n->y);
            if (cheapDist < minDist && n->connections.find(thisN) == n->connections.end()) {
                minDist = cheapDist;
                closest = thisN;
            }
        }
        if (closest) {
            makeConnection(skeleton, n, closest, 0);
        }
    }
    for (unsigned int i = 0; i < nodes.size(); i++) {
        nodes[i].done = false;
    }
    accessable.clear();
    depthSearch(&nodes[0]);
    for (unsigned int i = 0; i < nodes.size(); i++) {
        ZoneNode* n = &nodes[i];
        n->done = false;
        if (accessable.find(n) == accessable.end()) {
            int minDist = 1000000;
            ZoneNode* closest = NULL;
            for (std::set<ZoneNode*>::iterator j = accessable.begin(); j != accessable.end(); ++j) {
                ZoneNode* thisN = (ZoneNode*)(*j);
                int cheapDist = fabs(thisN->x - n->x) + fabs(thisN->y - n->y);
                if (cheapDist < minDist && n->connections.find(thisN) == n->connections.end()) {
                    minDist = cheapDist;
                    closest = thisN;
                }
            }
            if (closest) {
                makeConnection(skeleton, n, closest, 0);
            }
        }
    }
}



std::vector<unsigned char> bsdXs;
std::vector<unsigned char> bsdYs;
void Generator::bspRecurse(unsigned char* skeleton, BspGenNode* next, int left, std::vector<ZoneNode>* nodes) {
    int wid = next->wid;
    int hei = next->hei;
    left--;
    if (left > 0) {
        bool isVert;
        if (next->wid * 2 < next->hei) {
            isVert = false;
        } else if (next->hei * 2 > next->wid) {
            isVert = true;
        } else {
            isVert = rand() % 2;
        }

        BspGenNode* first = new BspGenNode;
        BspGenNode* secon = new BspGenNode;
        first->x = next->x;
        first->y = next->y;
        if (isVert) {
            int fWid = rand() % (wid / 2) + wid / 4;
            first->wid = fWid;
            first->hei = hei;
            secon->x = first->x + fWid;
            secon->y = next->y;
            secon->wid = wid - fWid;
            secon->hei = hei;
        } else {
            int fHei = rand() % (hei / 2) + hei / 4;
            first->wid = wid;
            first->hei = fHei;
            secon->x = next->x;
            secon->y = first->y + fHei;
            secon->wid = wid;
            secon->hei = hei - fHei;
        }
        next->first = first;
        next->secon = secon;
        bspRecurse(skeleton, next->first, left, nodes);
        bspRecurse(skeleton, next->secon, left, nodes);
        ZoneNode z1;
        z1.x = first->x + (first->wid / 2);
        z1.y = first->y + (first->hei / 2);
        z1.done = false;
        ZoneNode z2;
        z2.x = secon->x + (secon->wid / 2);
        z2.y = secon->y + (secon->hei / 2);
        z2.done = false;
        nodes->push_back(z1);
        nodes->push_back(z2);
        makeConnection(skeleton, &z1, &z2, 0);
        delete first;
        delete secon;
    } else {
        if (wid > 3 && hei > 3 && next->x >= 0 && next->y >= 0) {
            int w = wid / 2 - 1;
            int h = hei / 2 - 1;
            int x1 = next->x + rand() % w + 1;
            int y1 = next->y + rand() % h + 1;
            int x2 = next->x + wid - rand() % w - 1;
            int y2 = next->y + hei - rand() % h - 1;
            bool alt = !(rand() % 10);
            for (int i = x1; i < x2; i++) {
                for (int j = y1; j < y2; j++) {
                    if (alt) skeleton[i + j * toteWidth] = SKEL_ALT;
                    else skeleton[i + j * toteWidth] = SKEL_FLOOR;
                }
            }
        } else std::cout << "bahahahaha" << std::endl;
    }
}
void Generator::genSkeletonBsptree(unsigned char* skeleton, int x, int y, int width, int height) {
    std::vector<ZoneNode> nodes;
    BspGenNode mainNode;
    mainNode.x = x;
    mainNode.y = y;
    mainNode.wid = width;
    mainNode.hei = height;
    bspRecurse(skeleton, &mainNode, 5, &nodes);
}
