#include <vector>
#include "Area.h"
#include "Location.h"
#include "StatHolder.h"
using namespace std;

#ifndef WORLD_H
#define WORLD_H


class World: public StatHolder {
    public:
        World();
        virtual ~World();
        void addArea(Area* a);
        void setDim(int wid, int hei);
        int theTime;
    protected:
    private:
        int width;
        int height;
        vector<Area*> areas;
        Location* worldMap;
};

#endif // WORLD_H
