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
#include <queue>

/* This file handles the pathing alporithm. At the end it also does the fov stuffs. */

/*friend bool operator<(const loc &a, const loc &b) {
    return a.x * MAX_ZONE_SIZE + a.y < b.x * MAX_ZONE_SIZE + b.y;
}*/

// maybe change to jps?

Coord start;
Coord goal;
PathType pType;

struct Node {
    unsigned short g;
    unsigned short h;
    unsigned short f;
    unsigned char n;
    unsigned char height;
    Node* parent;
    Coord l;
    Node(Node* p, Coord where, int hei) : height(hei), parent(p), l(where) {
        refresh();
    }
    void refresh();
};

struct CompareNode : public std::binary_function<Node*, Node*, bool> {
	bool operator()(const Node* lhs, const Node* rhs) const {
		return lhs->f > rhs->f;
	}
};

int getG(Node* parent, Node* n) {
    int g;
    if ((parent->l.x - n->l.x) && (parent->l.y - n->l.y) /*If diagonal*/ ) {
        g = parent->g + 14;
    } else {
        g = parent->g + 10;
    }
    return g;
}

int getHFlee(Coord l) {
    Coord c = (l - goal).abs();
    if (c.x > c.y) {
        return -(c.y * 14 + (c.x - c.y) * 10);
    } else {
        return -(c.x * 14 + (c.y - c.x) * 10);
    }
}

int getH(Coord l) {
    if (pType == P_FLEE) return getHFlee(l);
    Coord c = (l - goal).abs();
    if (c.x > c.y) {
        return c.y * 14 + (c.x - c.y) * 10;
    } else {
        return c.x * 14 + (c.y - c.x) * 10;
    }
}

void Node::refresh() {
    if (parent) {
        g = getG(parent, this);
        n = parent->n + 1;
    } else {
        g = 0;
        n = 1;
    }
    h = getH(l);
    f = g + h;
}

std::pair<int, Node*> arr[64][64];

void Start::makePath(Unit* unit, Coord dest, Zone* zone, PathType pathingType) {
    int add = 0;
    if (dest.x == -1) {
        dest.x = 0; add = 1;
    } else if (dest.y < -1) {
        dest.y = 0; add = 2;
    } else if (dest.x == zone->getWidth()) {
        dest.x = zone->getWidth() - 1; add = 3;
    } else if (dest.y == zone->getHeight()) {
        dest.y = zone->getHeight() - 1; add = 4;
    }

    int swarm = unit->getStatValue(Stat::SWARM);
    int fly = unit->getStatValue(Stat::FLY);

    pType = pathingType;
    bool considerOtherUnits = pathingType != P_PASSUNITS;
    start = unit->pos;
    goal = dest;

    if (start.x == goal.x && start.y == goal.y) return;

    int ai = unit->getStatValue(Stat::AI);
    int wid = zone->getWidth();
    int hei = zone->getHeight();
    std::priority_queue<Node*, std::vector<Node*>, CompareNode> open;
    for (int i = 0; i < wid; i++) {
        for (int j = 0; j < hei; j++) {
            if (arr[i][j].second) {
                delete arr[i][j].second;
            }
            arr[i][j] = std::pair<int, Node*>(3, (Node*)NULL);
        }
    }
    arr[goal.x][goal.y].first = 0;
    open.push(new Node(NULL, start, zone->getLocationAt(start)->getTotalHeight()));
    arr[start.x][start.y] = std::pair<int, Node*>(1, open.top());
    Node* current = NULL;

    bool done = false;
    bool success = false;
    int iterations = 0;

    while(!done) {
        if (!open.empty()) {
            current = open.top();
            open.pop();
        }
        arr[current->l.x][current->l.y].first = 2;
        for (int i = 1; i < 10; i++) {
            if (i == 5) {
                i++;
            }
            Coord l = DIRS[i] + current->l;

            if (l.x < 0 || l.x >= wid || l.y < 0 || l.y >= hei) {
                continue;
            }
            int val = arr[l.x][l.y].first;
            if (val == 3) {
                //very long check to see if a location is walkable
                bool canDoor = ai == AI_HOSTILESMART;
                Location* locAt = zone->getLocationAt(l);
                int hei = locAt->getTotalHeight();
                int heightDiff;
                if (fly) heightDiff = 0;
                else heightDiff = hei - current->height;
                bool unitInWay = locAt->hasUnit();
                if (unitInWay) {
                    if (swarm && locAt->unit->getProto() == unit->getProto()) {
                        Swarmer* unitS = static_cast<Swarmer*>(unit);
                        Swarmer* nextS = static_cast<Swarmer*>(locAt->unit);
                        if (unitS->howMany() + nextS->howMany() <= swarm) unitInWay = false;
                    }
                }
                if ((locAt->isClosedDoor() && !canDoor) || heightDiff > 2 || heightDiff < -2 || hei == MAX_HEIGHT || Tile::get(locAt->tile)->blocksMove() ||
                        (considerOtherUnits && unitInWay && !(((pathingType != P_FLEE) && (l == dest))))) {
                    val = 2;
                }
            }
            if (val == 2) {
                continue;
            } else if (val == 1) {
                Node* n = arr[l.x][l.y].second;
                if (getG(current, n) < n->g) {
                    Node* temp  = n;
                    temp->parent = current;
                    temp->refresh();
                }
            } else {
                arr[l.x][l.y] = std::pair<int, Node*>(1, new Node(current, l, zone->getLocationAt(l)->getTotalHeight()));
                open.push(arr[l.x][l.y].second);
            }
        }
        iterations++;
        if (pathingType == P_FLEE) {
            if (iterations >= 50 || open.empty()) {
                done = true;
                success = true;
            }
        } else {
            if (arr[goal.x][goal.y].first == 2) {
                done = true;
                success = true;
            } else if (open.empty()) {
                done = true;
                success = false;
            }
        }
    }
    if (success) {
        int len1 = current->n;
        int len2 = len1;
        if (pathingType == P_STAIRS) len2++;
        if (add > 2) len2++;
        Coord* pathLocs = new Coord[len2];
        for (int i = len1 - 1; i >= 0; i--) {
            pathLocs[i] = current->l;
            current = current->parent;
        }
        if (pathingType == P_STAIRS) {
            pathLocs[len1] = Coord(-P_STAIRS, -P_STAIRS);
        }
        switch(add) {
            case 0: break;
            case 1: pathLocs[len1].y = pathLocs[len1 - 1].y; pathLocs[len1].x = -1; break;
            case 2: pathLocs[len1].x = pathLocs[len1 - 1].x; pathLocs[len1].y = -1; break;
            case 3: pathLocs[len1].y = pathLocs[len1 - 1].y; pathLocs[len1].x = zone->getWidth(); break;
            case 4: pathLocs[len1].x = pathLocs[len1 - 1].x; pathLocs[len1].y = zone->getHeight(); break;
            default: break;
        }
        Path* p = new Path;
        p->len = len2;
        p->pathLocs = pathLocs;
        p->cUnits = considerOtherUnits;
        if (unit->pointOnPath > -1) {
            delete[] unit->currentPath->pathLocs;
            delete unit->currentPath;
        }
        unit->currentPath = p;
        unit->pointOnPath = 0;
    } else {
        unit->currentPath = NULL;
        unit->pointOnPath = -1;
    }
}

fov_settings_type fovSettings;
Player* p;
unsigned char* vis;

void Start::myFovCircle(Zone* zone, void* source, Coord pos, int radius) {
    int center = pos.index(zone->getWidth());
    if (radius == 2) {
        visibilities[center + 1] = 2; visibilities[center - 1] = 2;
        visibilities[center + zone->getWidth() + 1] = 2; visibilities[center + zone->getWidth()] = 2; visibilities[center + zone->getWidth() - 1] = 2;
        visibilities[center - zone->getWidth() + 1] = 2; visibilities[center - zone->getWidth()] = 2; visibilities[center - zone->getWidth() - 1] = 2;
    } else if (radius == 3) {
        int nums[] = {center + 2 + zone->getWidth() * 2, center - 2 + zone->getWidth() * 2, center - 2 - zone->getWidth() * 2, center + 2 - zone->getWidth() * 2};
        int temp[4];
        for (int i = 0; i < 4; i++) {
            temp[i] = visibilities[nums[i]];
        }
        fov_circle(&fovSettings, zone, source, pos.x, pos.y, 3);
        for (int i = 0; i < 4; i++) {
            visibilities[nums[i]] = temp[i];
        }
    } else {
        fov_circle(&fovSettings, zone, source, pos.x, pos.y, radius);
    }
    visibilities[center] = 2;
}

void Start::myFovCirclePerm(Zone* zone, Coord pos, int radius, int mod) {
    if (radius == 2) {
        for (int i = 1; i < NUM_COORD_DIRS; i++) {
            if (i != 5) zone->getLocationAt(pos + DIRS[i])->light += mod;
        }
    } else if (radius == 3) {
        Location* locs[] = {zone->getLocationAt(pos + Coord(2, 2)), zone->getLocationAt(pos + Coord(2, -2)),
                            zone->getLocationAt(pos + Coord(-2, 2)), zone->getLocationAt(pos + Coord(-2, -2))};
        int temp[4];
        for (int i = 0; i < 4; i++) {
            temp[i] = locs[i]->light; //TODO ??????
        }
        fov_circle(&fovSettings, zone, &mod, pos.x, pos.y, 3);
        for (int i = 0; i < 4; i++) {
            locs[i]->light = temp[i];
        }
    } else {
        fov_circle(&fovSettings, zone, &mod, pos.x, pos.y, radius);
    }
    zone->getLocationAt(pos)->light += mod;
}

void Start::playerFieldOfView(bool isNew) {
    Zone* zone = player->getZone();
    int zWidth = zone->getWidth();
    int totes = zWidth * zone->getHeight();
    for (int i = 0; i < totes; i++) {
        if (!isNew && visibilities[i] >= 2) {
            Location* loc = zone->getLocationAt(i);
            int v = 0;
            unsigned char texs[2] = {7, 7};
            unsigned char locs[2] = {0, 0};
            if (loc->hasUnit()) {
                Graphic g = loc->unit->graphic;
                texs[v] = g.tex->getIndex();
                locs[v] = g.loc;
                v++;
            }
            if (loc->hasItems()) {
                Item* items = &(*loc->items)[0];
                int numItems = loc->items->size();
                for (int j = numItems - 1; j >= 0 && v < 2; j--) {
                    Graphic g = items[j].getType()->getGraphic(items[j].quantityCharge);
                    texs[v] = g.tex->getIndex();
                    locs[v] = g.loc;
                    v++;
                }
            }
            if (v < 2) {
                int struc = loc->structure;
                if (struc != S_NONE) {
                    texs[v] = structureTex->getIndex();
                    locs[v] = struc;
                    v++;
                }
            }
            if (v < 2 && loc->debris1) {
                texs[v] = splatterTex->getIndex();
                locs[v] = loc->debris1;
                v++;
            }
            if (v < 2 && loc->debris2) {
                texs[v] = splatterTex->getIndex();
                locs[v] = loc->debris2;
            }
            int x = i % zWidth;
            int y = i / zWidth;
            p->setMemory(Coord(x, y), texs[1], locs[1], texs[0], locs[0]);
        }
        visibilities[i] = 0;
    }
    Unit* unit = player->getUnit();
    int lightness = zone->getLightness();
    for (int i = 0; i < primeFolder->getEquips()->getNumItems(); i++) {
        int l = primeFolder->getEquips()->getItem(i)->getType()->getStatValue(Stat::LIGHT);
        if (l > lightness) lightness = l;
    }
    Coord p = unit->pos;
    int ui = p.index(zWidth);
    if (lightness == 11) lightness--;
    myFovCircle(zone, player->getUnit(), p, lightness);
    myFovCircle(zone, player, p, 12);
    visibilities[ui] = 2;
}

void fovApply(void* map, int x, int y, int dx, int dy, void* src) {
    Zone* z = static_cast<Zone*>(map);
    if (x >= 0 && y >= 0 && x < z->getWidth() && y < z->getHeight()) {
        int index = x + y * z->getWidth();
        if (src == p->getUnit()) {
            vis[index] = 2;
        } else if (src == p) {
            if (z->getLocationAt(Coord(x, y))->light > 0) {
                vis[index] = 2;
            } else if (vis[index] != 2) {
                vis[index] = 1;
            }
        } else {
            z->getLocationAt(Coord(x, y))->light += *(int*)src;
        }
    }
}

#define RANDA 14741
#define RANDB 7919
#define RANDC 44318
const int RANDM = rand();
const int RANDN = rand();
bool fovOpaque(void* map, int x, int y) {
    Zone* zone = static_cast<Zone*>(map);
    if (x < 0 || y < 0 || x >= zone->getWidth() || y >= zone->getHeight()) return true;
    Location* loc = zone->getLocationAt(Coord(x, y));
    if (Tile::get(loc->tile)->blocksLight()) {
        unsigned short r = (RANDA * x + RANDB * y + RANDC * zone->getFoon() + RANDM) ^ RANDN;
        return r % 50 > 24;
    }
    if (loc->isClosedDoor() || loc->structure == S_HIDDENDOOR) return true;
    return loc->height == MAX_HEIGHT;
}

void Start::initFieldOfView() {
    p = player;
    vis = visibilities;
    fov_settings_init(&fovSettings);
    fov_settings_set_opacity_test_function(&fovSettings, fovOpaque);
    fov_settings_set_apply_lighting_function(&fovSettings, fovApply);
    fov_settings_set_shape(&fovSettings, FOV_SHAPE_CIRCLE);
}

void Start::cleanFov() {
    fov_settings_free(&fovSettings);
    //fov_free(); ???
}
