#include <iostream>
using namespace std;
#include "Unit.h"
#include "Item.h"

#ifndef LOCATION_H
#define LOCATION_H

#define MAX_HEIGHT 31
enum Structures{S_WOODDOOR = 0, S_WOODDOOR_OPEN, S_WOODDOOR_BROKE, S_NORMDOOR, S_NORMDOOR_OPEN, S_NORMDOOR_BROKE, S_HIDDENDOOR, S_FOUNDDOOR,
    S_SKULLDOOR, S_SKULLDOOR_OPEN, S_SKULLDOOR_BROKE, S_STONEDOOR, S_STOREDOOR_OPEN, S_STONEDOOR_BROKE, S_WOODDOOR_LOCKED, S_placeholder,
    S_STAIRUP, S_STAIRDOWN, S_LADDERUP, S_LADDERDOWN, S_ROCK = 25, S_NONE = 31};

enum Fogs{FOG_NONE};

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

        unsigned short tile;
        unsigned char structure;

        unsigned char height;
        int getTotalHeight();

        short light;
        unsigned char fog1;
        unsigned char fog2;
        unsigned short debris1;
        unsigned short debris2;

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

        void clearStuff();
};

#endif // LOCATION_H
