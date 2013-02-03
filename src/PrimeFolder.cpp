#include "PrimeFolder.h"

PrimeFolder::PrimeFolder() {
    equips = new EquipmentFolder();
    ground = new GroundFolder();
    bag = new BagFolder(5);
    allThem[0] = Item(15);
    allThem[1] = Item(14);
    allThem[2] = Item(13);
}

PrimeFolder::~PrimeFolder() {
    delete bag;
    delete equips;
    delete ground;
}

int PrimeFolder::getNumItems() {
    return 3;
}

Item* PrimeFolder::getItem(int index) {
    return &allThem[index];
}

Item* PrimeFolder::getItems() {
    return allThem;
}

Item PrimeFolder::removeItem(int index) {
    return allThem[0];
}

bool PrimeFolder::addItem(Item* item) {
    return false;
}

BagFolder* PrimeFolder::getBag() {
    return bag;
}

EquipmentFolder* PrimeFolder::getEquips() {
    return equips;
}

GroundFolder* PrimeFolder::getGround() {
    return ground;
}

/*void PrimeFolder::setBag(BagFolder* b) {
    bag = b;
}*/
