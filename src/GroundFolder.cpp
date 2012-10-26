#include "GroundFolder.h"

EnvironmentManager* em;
void setEnvironmentManager(EnvironmentManager* ev) {
    em = ev;
}

GroundFolder::GroundFolder() {
    //first = true;
}

int GroundFolder::getNumItems() {
    return zone->getLocationAt(locX, locY)->items->size();
}
Item* GroundFolder::getItem(int index) {
    return &zone->getLocationAt(locX, locY)->items->at(index);
}
Item* GroundFolder::getItems() {
    return &zone->getLocationAt(locX, locY)->items->at(0);
}
Item GroundFolder::removeItem(int index) {
    //return location->removeItem(index);
    return em->removeItemFromPlace(locX, locY, zone, index);
}

void GroundFolder::setLocation(Zone* z, int x, int y) {
    zone = z;
    locX = x;
    locY = y;
}

bool GroundFolder::addItem(Item* item) {
    //location->addItem(*item);
    em->addItemToPlace(locX, locY, zone, *item);
    return true;
}
