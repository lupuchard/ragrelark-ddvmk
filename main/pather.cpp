#include "Start.h"
#include <queue>

/* This file handles the pathing alporithm. At the end it also does the fov stuffs. */
#define STRUCTURE_TEX 1
#define SPLATTERS_TEX 9

struct loc {
    short x;
    short y;
    loc(short x1, short y1) : x(x1), y(y1) { }

    friend bool operator<(const loc &a, const loc &b) {
        return a.x * MAX_ZONE_SIZE + a.y < b.x * MAX_ZONE_SIZE + b.y;
    }
};

loc start = loc(0, 0);
loc goal  = loc(0, 0);

struct node {
    unsigned short g;
    unsigned short h;
    unsigned short f;
    unsigned char n;
    unsigned char height;
    node* parent;
    loc l;
    node(node* p, loc where, int hei) : height(hei), parent(p), l(where) {
        refresh();
    }
    void refresh();
};

struct CompareNode : public std::binary_function<node*, node*, bool> {
	bool operator()(const node* lhs, const node* rhs) const {
		return lhs->f > rhs->f;
	}
};

int getG(node* parent, node* n) {
    int g;
    if ((parent->l.x - n->l.x) && (parent->l.y - n->l.y) /*If diagonal*/ ) {
        g = parent->g + 14;
    } else {
        g = parent->g + 10;
    }
    return g;
}

int getH(loc l) {
    int x = l.x - goal.x;
    int y = l.y - goal.y;
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    if (x > y) {
        return y * 14 + (x - y) * 10;
    } else {
        return x * 14 + (y - x) * 10;
    }
}

void node::refresh() {
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

pair<int, node*> arr[64][64];

void Start::makePath(Unit* unit, short xDest, short yDest, Zone* zone, int pathingType) {
    int add = 0;
    if (xDest == -1) {
        xDest = 0; add = 1;
    } else if (yDest < -1) {
        yDest = 0; add = 2;
    } else if (xDest == zone->getWidth()) {
        xDest = zone->getWidth() - 1; add = 3;
    } else if (yDest == zone->getHeight()) {
        yDest = zone->getHeight() - 1; add = 4;
    }

    bool considerOtherUnits = pathingType != P_PASSUNITS;
    start = loc(unit->x, unit->y);
    goal = loc(xDest, yDest);

    if (start.x == goal.x && start.y == goal.y) return;

    int ai = unit->getStatValue(S_AI);
    int wid = zone->getWidth();
    int hei = zone->getHeight();
    priority_queue<node*, vector<node*>, CompareNode> open;
    for (int i = 0; i < wid; i++) {
        for (int j = 0; j < hei; j++) {
            if (arr[i][j].second) {
                delete arr[i][j].second;
            }
            arr[i][j] = pair<int, node*>(3, (node*)NULL);
        }
    }
    arr[goal.x][goal.y].first = 0;
    open.push(new node(NULL, start, zone->getLocationAt(start.x, start.y)->getTotalHeight()));
    arr[start.x][start.y] = pair<int, node*>(1, open.top());
    node* current = NULL;

    bool done = false;
    bool success = false;

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
            loc l = loc(xDirs[i] + current->l.x, yDirs[i] + current->l.y);

            if (l.x < 0 || l.x >= wid || l.y < 0 || l.y >= hei) {
                continue;
            }
            int val = arr[l.x][l.y].first;
            if (val == 3) {
                bool canDoor = ai > 1;
                Location* locAt = zone->getLocationAt(l.x, l.y);
                int hei = locAt->getTotalHeight();
                int heightDiff = hei - current->height;
                if ((locAt->isClosedDoor() && !canDoor) || heightDiff > 2 || heightDiff < -2 || hei == MAX_HEIGHT || (considerOtherUnits && locAt->hasUnit() && (l.x != xDest || l.y != yDest))) {
                    val = 2;
                }
            }
            if (val == 2) {
                continue;
            } else if (val == 1) {
                node* n = arr[l.x][l.y].second;
                if (getG(current, n) < n->g) {
                    node* temp  = n;
                    temp->parent = current;
                    temp->refresh();
                }
            } else {
                arr[l.x][l.y] = pair<int, node*>(1, new node(current, l, zone->getLocationAt(l.x, l.y)->getTotalHeight()));
                open.push(arr[l.x][l.y].second);
            }
        }
        if (arr[goal.x][goal.y].first == 2) {
			done = true;
			success = true;
		} else if (open.empty()) {
			done = true;
			success = false;
		}
    }
    if (success) {
        int len1 = current->n;
        int len2 = len1;
        if (pathingType == P_STAIRS) len2++;
        if (add > 2) len2++;
        short* pathXs = new short[len2];
        short* pathYs = new short[len2];
        for (int i = len1 - 1; i >= 0; i--) {
            pathXs[i] = current->l.x;
            pathYs[i] = current->l.y;
            current = current->parent;
        }
        if (pathingType == P_STAIRS) {
            pathXs[len1] = -P_STAIRS;
            pathYs[len1] = -P_STAIRS;
        }
        switch(add) {
            case 0: break;
            case 1: pathYs[len1] = pathYs[len1 - 1]; pathXs[len1] = -1; break;
            case 2: pathXs[len1] = pathXs[len1 - 1]; pathYs[len1] = -1; break;
            case 3: pathYs[len1] = pathYs[len1 - 1]; pathXs[len1] = zone->getWidth(); break;
            case 4: pathXs[len1] = pathXs[len1 - 1]; pathYs[len1] = zone->getHeight(); break;
            default: break;
        }
        path* p = new path;
        p->len = len2;
        p->pathXs = pathXs;
        p->pathYs = pathYs;
        p->cUnits = considerOtherUnits;
        if (unit->pointOnPath > -1) {
            delete[] unit->currentPath->pathXs;
            delete[] unit->currentPath->pathYs;
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

void Start::myFovCircle(Zone* zone, void* source, int x, int y, int radius) {
    int center = x + y * zone->getWidth();
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
        fov_circle(&fovSettings, zone, source, x, y, 3);
        for (int i = 0; i < 4; i++) {
            visibilities[nums[i]] = temp[i];
        }
    } else {
        fov_circle(&fovSettings, zone, source, x, y, radius);
    }
    visibilities[center] = 2;
}

void Start::myFovCirclePerm(Zone* zone, int x, int y, int radius, int mod) {
    if (radius == 2) {
        zone->getLocationAt(x + 1, y)->light += mod; zone->getLocationAt(x - 1, y)->light += mod;
        zone->getLocationAt(x, y + 1)->light += mod; zone->getLocationAt(x, y - 1)->light += mod;
        zone->getLocationAt(x + 1, y + 1)->light += mod; zone->getLocationAt(x + 1, y - 1)->light += mod;
        zone->getLocationAt(x - 1, y + 1)->light += mod; zone->getLocationAt(x - 1, y - 1)->light += mod;
    } else if (radius == 3) {
        Location* locs[] = {zone->getLocationAt(x + 2, y + 2), zone->getLocationAt(x + 2, y - 2),
                            zone->getLocationAt(x - 2, y + 2), zone->getLocationAt(x - 2, y - 2)};
        int temp[4];
        for (int i = 0; i < 4; i++) {
            temp[i] = locs[i]->light;
        }
        fov_circle(&fovSettings, zone, &mod, x, y, 3);
        for (int i = 0; i < 4; i++) {
            locs[i]->light = temp[i];
        }
    } else {
        fov_circle(&fovSettings, zone, &mod, x, y, 3);
    }
    zone->getLocationAt(x, y)->light += mod;
}

void Start::playerFieldOfView(bool isNew) {
    Zone* zone = player->getZone();
    int zWidth = zone->getWidth();
    int totes = zWidth * zone->getHeight();
    for (int i = 0; i < totes; i++) {
        if (!isNew && visibilities[i] >= 2) {
            int x = i % zWidth;
            int y = i / zWidth;
            Location* loc = zone->getLocationAt(x, y);
            int v = 0;
            unsigned char texs[2] = {7, 7};
            unsigned char locs[2] = {0, 0};
            if (loc->hasUnit()) {
                graphic g = loc->unit->getGraphic();
                texs[v] = g.tex;
                locs[v] = g.loc;
                v++;
            }
            vector<graphic> itemGraphics;
            Item* items = &(*loc->items)[0];
            int numItems = loc->items->size();
            for (int j = numItems - 1; j >= 0 && v < 2; j--) {
                graphic g = getItemType(items[j].itemType)->getGraphic(items[j].quantityCharge);
                texs[v] = g.tex;
                locs[v] = g.loc;
                v++;
            }
            if (v < 2) {
                int struc = loc->structure;
                if (struc != S_NONE) {
                    texs[v] = STRUCTURE_TEX;
                    locs[v] = struc;
                    v++;
                }
            }
            if (v < 2 && splatters[i] != 255) {
                texs[v] = SPLATTERS_TEX;
                locs[v] = splatters[i];
            }
            p->setMemory(x, y, texs[1], locs[1], texs[0], locs[0]);
        }
        visibilities[i] = 0;
    }
    Unit* unit = player->getUnit();
    int lightness = zone->getLightness();
    for (int i = 0; i < primeFolder->getEquips()->getNumItems(); i++) {
        int l = getItemType(primeFolder->getEquips()->getItem(i)->itemType)->getStatValue(S_LIGHT);
        if (l > lightness) lightness = l;
    }
    int px = unit->x;
    int py = unit->y;
    int ui = px + py * zWidth;
    if (lightness == 11) lightness--;
    myFovCircle(zone, player->getUnit(), px, py, lightness);
    myFovCircle(zone, player, px, py, 12);
    visibilities[ui] = 2;
}

void fovApply(void* map, int x, int y, int dx, int dy, void* src) {
    Zone* z = (Zone*)map;
    if (x >= 0 && y >= 0 && x < z->getWidth() && y < z->getHeight()) {
        int index = x + y * z->getWidth();
        if (src == p->getUnit()) {
            vis[index] = 2;
        } else if (src == p) {
            if (z->getLocationAt(x, y)->light > 0) {
                vis[index] = 2;
            } else if (vis[index] != 2) {
                vis[index] = 1;
            }
        } else {
            z->getLocationAt(x, y)->light += *(int*)src;
        }
    }
}

bool fovOpaque(void* map, int x, int y) {
    Zone* zone = (Zone*)map;
    if (x < 0 || y < 0 || x >= zone->getWidth() || y >= zone->getHeight()) return true;
    Location* loc = zone->getLocationAt(x, y);
    if (loc->isClosedDoor()) return true;
    return loc->height == MAX_HEIGHT;
}

void Start::initFieldOfView() {
    p = player;
    vis = visibilities;
    fov_settings_init(&fovSettings);
    fov_settings_set_opacity_test_function(&fovSettings, fovOpaque);
    fov_settings_set_apply_lighting_function(&fovSettings, fovApply);
}

void Start::cleanFov() {
    fov_settings_free(&fovSettings);
}
