#ifndef AREA_H
#define AREA_H

#include <string>
#include <vector>
#include "StatHolder.h"
#include "DungeonStack.h"
using namespace std;

#define NUM_ESCAPE_DIRS 3

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
        Area(int width);
        virtual ~Area();

        void setName(string name);

        void addZone(Zone* zone);
        unsigned int getNumZones();
        Zone* getZone(int i);

        void addDungeonStack(DungeonStack* dungeonStack);
        unsigned int getNumDungeonStacks();
        DungeonStack* getDungeonStack(int stackIndex);

        void addConnection(connection c);
        speLoc moveConnection(Zone* zone, int x, int y);
        void changeConnection(Zone* zone, int x, int y, int newX, int newY);
        speLoc moveZones(Zone* z, int x, int y);
    protected:
    private:
        string name;
        vector<Zone*> zones;
        vector<DungeonStack*> dungeonStacks;
        vector<connection> connections;
        int numConnections;
        int width;
};

#endif // AREA_H
