
#include "Start.h"

/*tilesettypes:
all=>
 0-main floor
 1-floor2 (20%)
 2-floor3 (20%)
 3-floor4 (5%)
 4-main wall
 5-wall2 (20%)
 6-wall3 (20%)
 7-wall4 (5%)
 8-ash
 16-water
 17-ice
 18-lava
 19-obsidion
*/

/*
10000-eeverywhere
3500-everywhere
1000-very common
350-common
100-uncommon
35-rare
10-very rare
3-extremely rare
1-rrare
0-nonexistant
*/

struct ZONODE {
    short x;
    short y;
    bool done;
    set<ZONODE*> connections;
};

typedef struct ZONODE zoneNode;

struct BSPGEN {
    BSPGEN* first; //left and up
    BSPGEN* secon; //right and down
    unsigned char x;
    unsigned char y;
    unsigned char wid;
    unsigned char hei;
};

typedef struct BSPGEN bspGenNode;

void placeWall(Location* loc) {
    int foo = rand() % 20;
    if (foo == 0) {
        loc->tile = 7;
    } else if (foo < 4) {
        loc->tile = 6;
    } else if (foo < 8) {
        loc->tile = 5;
    } else {
        loc->tile = 4;
    }
    loc->height = MAX_HEIGHT;
}
void placeFloor(Location* loc) {
    int foo = rand() % 20;
    if (foo == 0) {
        loc->tile = 3;
    } else if (foo < 4) {
        loc->tile = 2;
    } else if (foo < 8) {
        loc->tile = 1;
    } else {
        loc->tile = 0;
    }
    loc->height = MAX_HEIGHT / 2;
}

void makeConnection(zoneNode* n1, zoneNode* n2, Zone* zone, int type) {
    if (n1->connections.find(n2) != n1->connections.end()) return; //if a connection already exists dont do anything
    if (!type) type = rand() % 2 + 1;
    if (type == 2) {
        zoneNode* temp = n1;
        n1 = n2;
        n2 = temp;
    }
    if (n1->x < n2->x) {
        for (int i = n1->x; i <= n2->x; i++) {
            Location* loc = zone->getLocationAt(i, n2->y);
            loc->tile = 0;
        }
    } else if (n1->x > n2->x) {
        for (int i = n2->x; i <= n1->x; i++) {
            Location* loc = zone->getLocationAt(i, n2->y);
            loc->tile = 0;
        }
    }
    if (n1->y < n2->y) {
        for (int i = n1->y; i <= n2->y; i++) {
            Location* loc = zone->getLocationAt(n1->x, i);
            loc->tile = 0;
        }
    } else if (n1->y > n2->y) {
        for (int i = n2->y; i <= n1->y; i++) {
            Location* loc = zone->getLocationAt(n1->x, i);
            loc->tile = 0;
        }
    }
    n1->connections.insert(n2);
    n2->connections.insert(n1);
}

set<zoneNode*> accessable;

void depthSearch(zoneNode* start) {
    start->done = true;
    accessable.insert(start);

    int j = 0;
    for (set<zoneNode*>::iterator i = start->connections.begin(); i != start->connections.end(); i++) {
        zoneNode* n = (zoneNode*)(*i);
        if (!n->done) {
            depthSearch(n);
        }

        j++;
        if (j > 1000) {
            cout << "MEGALOOP depthSearch" << endl;
        }
    }
}

void dooors(int sx, int sy, int ex, int ey, Zone* zone, int uncomminity) {
    for (int i = sx + 1; i < ex - 1; i++) {
        for (int j = sy + 1; j < ey - 1; j++) {
            Location* loc = zone->getLocationAt(i, j);
            if (loc->height != MAX_HEIGHT) {
                Location* nLoc = zone->getLocationAt(i, j - 1);
                Location* sLoc = zone->getLocationAt(i, j + 1);
                Location* eLoc = zone->getLocationAt(i + 1, j);
                Location* wLoc = zone->getLocationAt(i - 1, j);
                if (nLoc->height == MAX_HEIGHT && sLoc->height == MAX_HEIGHT) {
                    if (eLoc->height == MAX_HEIGHT || wLoc->height == MAX_HEIGHT || eLoc->structure == S_WOODDOOR || wLoc->structure == S_WOODDOOR) {
                        continue;
                    }
                } else if (eLoc->height == MAX_HEIGHT && wLoc->height == MAX_HEIGHT) {
                    if (nLoc->height == MAX_HEIGHT || sLoc->height == MAX_HEIGHT || nLoc->structure == S_WOODDOOR || sLoc->structure == S_WOODDOOR) {
                        continue;
                    }
                } else {
                    continue;
                }
                Location* seLoc = zone->getLocationAt(i + 1, j + 1);
                Location* neLoc = zone->getLocationAt(i + 1, j - 1);
                Location* swLoc = zone->getLocationAt(i - 1, j + 1);
                Location* nwLoc = zone->getLocationAt(i - 1, j - 1);
                if (seLoc->height == MAX_HEIGHT && neLoc->height == MAX_HEIGHT && swLoc->height == MAX_HEIGHT && nwLoc->height == MAX_HEIGHT) {
                    continue;
                }
                if (!(rand() % uncomminity)) {
                    loc->structure = S_WOODDOOR;
                }
            }
        }
    }
}

vector<unsigned char> bsdXs;
vector<unsigned char> bsdYs;

void fooey(Zone* zone, bspGenNode* next, int left, vector<zoneNode>* nodes) {
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

        bspGenNode* first = new bspGenNode;
        bspGenNode* secon = new bspGenNode;
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
        fooey(zone, next->first, left, nodes);
        fooey(zone, next->secon, left, nodes);
        zoneNode z1;
        z1.x = first->x + (first->wid / 2);
        z1.y = first->y + (first->hei / 2);
        z1.done = false;
        zoneNode z2;
        z2.x = secon->x + (secon->wid / 2);
        z2.y = secon->y + (secon->hei / 2);
        z2.done = false;
        nodes->push_back(z1);
        nodes->push_back(z2);
        makeConnection(&z1, &z2, zone, 0);
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
            for (int i = x1; i < x2; i++) {
                for (int j = y1; j < y2; j++) {
                    Location* loc = zone->getLocationAt(i, j);
                    loc->tile = 0;
                }
            }
        } else {
            cout << "bahahahaha" << endl;
        }
    }
}

vector<pair<int, int> > gZoneRec(Zone* zone, int tilesetType, int genType, int sx, int sy, int ex, int ey) {
    zone->fill();
    int wid = ex - sx;
    int hei = ey - sy;
    vector<zoneNode> nodes;
    vector<pair<int, int> > possibleLocs;
    switch(genType) {
        case G_NONE: {
            for (int i = sx; i < ex; i++) {
                for (int j = sy; j < ey; j++) {
                    Location* loc = zone->getLocationAt(i, j);
                    loc->structure = S_NONE;
                    if (i == sx || i == ex || sy || j == ey) {
                        loc->tile = 0;
                        loc->height = MAX_HEIGHT;
                    } else {
                        if (rand() % 20) {
                            loc->tile = 1;
                            loc->height = MAX_HEIGHT / 2;
                        } else {
                            loc->tile = 0;
                            loc->height = MAX_HEIGHT;
                        }
                    }
                }
            }
        } break;
        case G_MYDUN: {
            for (int x = sx; x < ex; x++) {
                for (int y = sy; y < ey; y++) {
                    Location* loc = zone->getLocationAt(x, y);
                    loc->tile = 1;
                    loc->structure = S_NONE;
                }
            }

            int numRooms = wid * hei / (rand() % 40 + 60);

            for (int k = 0; k < numRooms; k++) {
                int sizeX = rand() % 4 + 2;
                int sizeY = rand() % 4 + 2;
                int xLoc = rand() % (wid - 8) + 4 + sx;
                int yLoc = rand() % (hei - 8) + 4 + sy;
                int x1 = xLoc - sizeX / 2;
                int y1 = yLoc - sizeY / 2;
                int x2 = xLoc + sizeX / 2;
                int y2 = yLoc + sizeY / 2;
                if (sizeX % 2) x2++;
                if (sizeY % 2) y2++;
                for (int i = x1; i <= x2; i++) {
                    for (int j = y1; j <= y2; j++) {
                        Location* loc = zone->getLocationAt(i, j);
                        loc->tile = 0;
                    }
                }
                zoneNode n;
                n.x = xLoc;
                n.y = yLoc;
                nodes.push_back(n);
            }

            for (int i = 0; i < zone->getNumReserved(); i++) {
                pair<int, int> r = zone->getRes(i);
                zoneNode n;
                n.x = r.first;
                n.y = r.second;
                nodes.push_back(n);
            }

            for (int i = 0; i < numRooms; i++) {
                zoneNode* n = &nodes[rand() % nodes.size()];
                int minDist = 1000000;
                zoneNode* closest = NULL;
                for (unsigned int j = 0; j < nodes.size(); j++) {
                    zoneNode* thisN = &nodes[j];
                    int cheapDist = fabs(thisN->x - n->x) + fabs(thisN->y - n->y);
                    if (cheapDist < minDist && n->connections.find(thisN) == n->connections.end()) {
                        minDist = cheapDist;
                        closest = thisN;
                    }
                }
                if (closest) {
                    makeConnection(n, closest, zone, 0);
                }
            }

            for (unsigned int i = 0; i < nodes.size(); i++) {
                nodes[i].done = false;
            }
            accessable.clear();
            depthSearch(&nodes[0]);
            for (unsigned int i = 0; i < nodes.size(); i++) {
                zoneNode* n = &nodes[i];
                n->done = false;
                if (accessable.find(n) == accessable.end()) {
                    int minDist = 1000000;
                    zoneNode* closest = NULL;
                    for (set<zoneNode*>::iterator j = accessable.begin(); j != accessable.end(); j++) {
                        zoneNode* thisN = (zoneNode*)(*j);
                        int cheapDist = fabs(thisN->x - n->x) + fabs(thisN->y - n->y);
                        if (cheapDist < minDist && n->connections.find(thisN) == n->connections.end()) {
                            minDist = cheapDist;
                            closest = thisN;
                        }
                    }
                    if (closest) {
                        makeConnection(n, closest, zone, 0);
                    }
                }
            }

            for (int x = sx; x < ex; x++) {
                for (int y = sy; y < ey; y++) {
                    Location* loc = zone->getLocationAt(x, y);
                    if (loc->tile == 1) {
                        placeWall(loc);
                    } else {
                        placeFloor(loc);
                        possibleLocs.push_back(pair<int, int>(x, y));
                    }
                }
            }

            dooors(sx, sy, ex, ey, zone, 5);
        } break;
        case G_BSP: {
            for (int x = sx; x < ex; x++) {
                for (int y = sy; y < ey; y++) {
                    Location* loc = zone->getLocationAt(x, y);
                    loc->tile = 1;
                    loc->structure = S_NONE;
                }
            }

            bspGenNode mainNode;
            mainNode.x = sx;
            mainNode.y = sy;
            mainNode.wid = ex - sx;
            mainNode.hei = ey - sy;
            fooey(zone, &mainNode, 5, &nodes);

            for (int i = 0; i < zone->getNumReserved(); i++) {
                pair<int, int> r = zone->getRes(i);
                Location* loc = zone->getLocationAt(r.first, r.second);
                if (loc->tile == 1) {
                    zoneNode n;
                    n.x = r.first;
                    n.y = r.second;
                    int minDist = 1000000;
                    zoneNode* closest = NULL;
                    for (unsigned int j = 0; j < nodes.size(); j++) {
                        zoneNode* thisN = &nodes[j];
                        int cheapDist = fabs(thisN->x - n.x) + fabs(thisN->y - n.y);
                        if (cheapDist < minDist) {
                            minDist = cheapDist;
                            closest = thisN;
                        }
                    }
                    nodes.push_back(n);
                    if (closest) {
                        makeConnection(&nodes[nodes.size() - 1], closest, zone, 0);
                    }
                }
            }

            for (int x = sx; x < ex; x++) {
                for (int y = sy; y < ey; y++) {
                    Location* loc = zone->getLocationAt(x, y);
                    if (loc->tile == 1) {
                        placeWall(loc);
                    } else {
                        placeFloor(loc);
                        possibleLocs.push_back(pair<int, int>(x, y));
                    }
                }
            }

            dooors(sx, sy, ex, ey, zone, 2);
        } break;
        case G_DUNGEON: {
            int num = rand() % 9;
            if (num) {
                vector<pair<int, int> > moreLocs;
                if (num % 2) {
                    moreLocs = gZoneRec(zone, tilesetType, G_MYDUN, sx, sy, ex, ey);
                } else {
                    moreLocs = gZoneRec(zone, tilesetType, G_BSP, sx, sy, ex, ey);
                }
                possibleLocs.insert(possibleLocs.end(), moreLocs.begin(), moreLocs.end());
            } else {
                int mx = (ex - sx) / 2 + sx;
                vector<pair<int, int> > moreLocs = gZoneRec(zone, tilesetType, G_MYDUN, sx, sy, mx, ey);
                possibleLocs.insert(possibleLocs.end(), moreLocs.begin(), moreLocs.end());
                moreLocs = gZoneRec(zone, tilesetType, G_BSP, mx, sy, ex, ey);
                possibleLocs.insert(possibleLocs.end(), moreLocs.begin(), moreLocs.end());
            }
        } break;
        default: break;
    }
    return possibleLocs;
}

void Start::generateZone(Zone* zone, int tilesetType, int zoneType, int sx, int sy, int ex, int ey) {
    vector<pair<int, int> > possibleLocs = gZoneRec(zone, tilesetType, zoneType, sx, sy, ex, ey);
    int numEncounters = zone->getWidth() * zone->getHeight() / 200 + rand() % 3;
    createEncounters(zone, numEncounters, possibleLocs);
    int numItems = zone->getWidth() * zone->getHeight() / 100 + rand() % 3;
    createItems(zone, numItems, possibleLocs);
    zone->becomeGenned();
}
