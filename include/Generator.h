#ifndef GENERATOR_H
#define GENERATOR_H

#include <vector>
#include "Zone.h"

using namespace std;

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
    set<ZONODE*> connections;
};
typedef struct ZONODE ZoneNode;

class Generator {
    public:
        Generator();
        virtual ~Generator();

        unsigned char* genSkeleton(int width, int height, GenType genType);
        void fillSkeleton(unsigned char* skeleton, Zone* zone, Tile** tiles);
    protected:
    private:
        void genSkeletonCrazy(unsigned char* skeleton, int x, int y, int width, int height);
        void genSkeletonBsptree(unsigned char* skeleton, int x, int y, int width, int height);

        void makeConnection(unsigned char* skeleton, ZoneNode* n1, ZoneNode* n2, int type);
        void bspRecurse(unsigned char* skeleton, BspGenNode* next, int left, vector<ZoneNode>* nodes);

        int toteWidth;
        int toteHeight;
};

#endif // GENERATOR_H
