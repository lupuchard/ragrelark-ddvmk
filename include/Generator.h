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

#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include "Zone.h"

enum {SKEL_FLOOR, SKEL_WALL, SKEL_DOOR, SKEL_STAIRS, SKEL_WATER, SKEL_ALT, SKEL_PIT, SKEL_HIGH, SKEL_SLOPE};

struct BSPGEN {
    BSPGEN* first; //left and up
    BSPGEN* secon; //right and down
    unsigned char x;
    unsigned char y;
    unsigned char wid;
    unsigned char hei;
};
typedef struct BSPGEN BspGenNode;

struct ZONODE {
    short x;
    short y;
    bool done;
    std::set<ZONODE*> connections;
};
typedef struct ZONODE ZoneNode;

class Generator {
    public:
        Generator();
        virtual ~Generator();

        unsigned char* genSkeleton(int width, int height, GenType genType);
        void fillSkeleton(unsigned char* skeleton, Zone* zone, TileSet* tiles);
    private:
        void genSkeletonCrazy(unsigned char* skeleton, int x, int y, int width, int height);
        void genSkeletonBsptree(unsigned char* skeleton, int x, int y, int width, int height);

        void makeConnection(unsigned char* skeleton, ZoneNode* n1, ZoneNode* n2, int type);
        void bspRecurse(unsigned char* skeleton, BspGenNode* next, int left, std::vector<ZoneNode>* nodes);

        int toteWidth;
        int toteHeight;
};

#endif // GENERATOR_H
