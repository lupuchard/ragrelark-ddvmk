#include "Player.h"

Player::Player() {
    //zone = z;
    //unit = u;
}

Player::~Player() {
    delete unit;
    for (map<Zone*, zoneMemory*>::iterator i = memoryBank.begin(); i != memoryBank.end(); i++) {
        zoneMemory* m = i->second;
        delete[] m->bottomTex;
        delete[] m->bottomLoc;
        delete[] m->topTex;
        delete[] m->topLoc;
        delete m;
    }
}

Zone* Player::getZone() {
    return zone;
}

Area* Player::getArea() {
    return area;
}

Unit* Player::getUnit() {
    return unit;
}

void Player::setZone(Zone* z) {
    zone = z;
    map<Zone*, zoneMemory*>::iterator it = memoryBank.find(z);
    if (it == memoryBank.end()) {
        int len = z->getWidth() * z->getHeight();
        zoneMemory* newZoneMemory = new zoneMemory;
        memoryBank[z] = newZoneMemory;
        newZoneMemory->bottomTex = new unsigned char[len];
        newZoneMemory->bottomLoc = new unsigned char[len];
        newZoneMemory->topTex = new unsigned char[len];
        newZoneMemory->topLoc = new unsigned char[len];
        for (int i = 0; i < len; i++) {
            newZoneMemory->bottomTex[i] = 5; //5 is the index of the menu texture, used to specify no graphic at this point
            newZoneMemory->topTex[i] = 5;
        }
        currentZoneMemory = newZoneMemory;
    } else {
        currentZoneMemory = it->second;
    }
}

pair<int, int> Player::getMemoryTop(int x, int y) {
    int i = x + y * zone->getWidth();
    return pair<int, int>(currentZoneMemory->topTex[i], currentZoneMemory->topLoc[i]);
}

pair<int, int> Player::getMemoryBottom(int x, int y) {
    int i = x + y * zone->getWidth();
    return pair<int, int>(currentZoneMemory->bottomTex[i], currentZoneMemory->bottomLoc[i]);
}

void Player::setMemory(int x, int y, unsigned char bt, unsigned char bl, unsigned char tt, unsigned char tl) {
    int i = x + y * zone->getWidth();
    currentZoneMemory->bottomTex[i] = bt;
    currentZoneMemory->bottomLoc[i] = bl;
    currentZoneMemory->topTex[i] = tt;
    currentZoneMemory->topLoc[i] = tl;
}

void Player::setArea(Area* a) {
    area = a;
}

void Player::setUnitProto(StatHolder* proto) {
    unit = new Unit(name, proto);
}

void Player::setName(string n) {
    name = n;
}
