#include <string>
#include <vector>
#include "Unit.h"
#include "StatHolder.h"
#include "Zone.h"
using namespace std;

#ifndef AREA_H
#define AREA_H

#define NUM_ESCAPE_DIRS 3

/*
A|B |C |D
-+--+--+-
E|FQ|GR|H
-+--+--+-
I|JS|KT|L
-+--+--+-
M|N |O |P
*/

//                        X   A   B   C   D   E   H   I   L   M   N   O   P  AB  BC  CD  DH  HL  LP  PO  ON  NM  MI  IE  EA ABC BCD DHL HLP MNO NOP AEI EIM  ABCD DHLP MNOP AEIM
//const char xRanges1[] = {0, -1,  0,  1,  2, -1,  2, -1,  2, -1,  0,  1,  2, -1,  0,  1,  2,  2,  2,  1,  0, -1, -1, -1, -1, -1,  0, -1, -1, -1,  0, -1, -1,  -1,   2,  -1,  -1};
//const char xRanges2[] = {0,  0,  1,  2,  3,  0,  3,  0,  3,  0,  1,  2,  3,  1,  2,  3,  3,  3,  3,  3,  2,  1,  0,  0,  0,  2,  3,  0,  0,  2,  3,  0,  0,   3,   3,   3,   0};
//const char yRanges1[] = {0, -1, -1, -1, -1,  0,  0,  1,  1,  2,  2,  2,  2, -1, -1, -1, -1,  0,  1,  2,  2,  2,  1,  0, -1, -1, -1, -1,  0,  2,  2, -1,  0,  -1,  -1,   2,  -1};
//const char yRanges2[] = {0,  0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  3,  3,  0,  0,  0,  1,  2,  3,  3,  3,  3,  3,  2,  1,  0,  0,  2,  3,  3,  3,  2,  3,   0,   3,   3,   3};
//const bool setRanX[] = {fa, tr, fa, fa, tr, tr, tr, tr, tr, tr, fa, fa, tr, fa, fa, fa, tr, tr, tr, fa, fa, fa, tr, tr, tr, fa, fa, tr, tr, fa, fa, tr, tr,  fa,  tr,  fa,  tr};
//const bool setRanY[] = {fa, tr, tr, tr, tr, fa, fa, fa, fa, tr, tr, tr, tr, tr, tr, tr, fa, fa, fa, tr, tr, tr, fa, fa, fa, tr, tr, fa, fa, tr, tr, fa, fa,  tr,  fa,  tr,  fa};

typedef struct {
    short x1;
    short y1;
    short x2;
    short y2;
    Zone* z1;
    Zone* z2;
} connection;

typedef struct {
    short x;
    short y;
    Zone* z;
} speLoc;

void setAreaFormUser(FormulaUser* formUser);

class Area: public StatHolder {
    public:
        Area(int width, int height);
        virtual ~Area();
        void setZones(Zone** zones, int numZones);
        void setName(string name);
        speLoc moveZones(Zone* z, int x, int y);
        void addConnection(connection c);
        speLoc moveConnection(Zone* zone, int x, int y);
        unsigned short getNumZones();
        Zone* getZone(int i);
    protected:
    private:
        string name;
        Zone** zones;
        unsigned short numZones;
        vector<connection> connections;
        unsigned short numConnections;
        unsigned char width;
        unsigned char height;
        //char weather;
};

#endif // AREA_H
