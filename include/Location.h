#include <iostream>
using namespace std;
#include "Unit.h"
#include "Item.h"

#ifndef LOCATION_H
#define LOCATION_H

#define MAX_HEIGHT 31
#define S_NONE 31
#define S_WOODDOOR 0
#define S_STAIRUP 16
#define S_STAIRDOWN 17
#define S_ROCK 25

/*struct itemNode {
    Item item;
    itemNode* nextItem;
};*/

bool isOpen(int str);
bool isClosedDoor(int str);
bool isOpenDoor(int str);

class Location {
    public:
        Location();
        Location(int h);

        unsigned char height;
        unsigned char tile;
        unsigned char structure;
        unsigned char light;
        int getTotalHeight();

        Unit* unit;
        bool hasUnit();
        void removeUnit();

        bool isOpen();
        bool isClosedDoor();
        bool isOpenDoor();

        vector<Item>* items;
        bool addItem(Item item);
        Item removeItem(int itemI);
        bool hasItems();
};

#endif // LOCATION_H
