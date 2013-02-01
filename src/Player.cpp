#include "Player.h"

int calcXpReq(int level, float mult) {
    return (int)(pow(level + 1, 1.1) * mult);
}

Player::Player(PrimeFolder* pFolder) {
    for (int i = 0; i < NUM_SKILLS; i++) {
        skillExps[i] = 0;
        skillLevels[i] = 0;
    }
    primeFolder = pFolder;

    //trainSpell(SPELL_LIGHT, 100);
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
    xpBank = 0;
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

int Player::gainSkillExp(SkillType skill, int xpGained) {
    skillExps[skill] += xpGained;
    if (skillExps[skill] < 0) {
        if (skillLevels[skill] >= 0) {
            int prevXpReq;
            if (skillLevels[skill] == 1) prevXpReq = 100;
            else prevXpReq = calcXpReq(skillLevels[skill] - 1, 30.f);
            skillExps[skill] += prevXpReq;
            skillLevels[skill]--;
            return -1;
        }
        return 0;
    }
    int xpReq = calcXpReq(skillLevels[skill], 30.f);
    int fooey = 0;
    while (skillExps[skill] >= xpReq) {
        skillExps[skill] -= xpReq;
        skillLevels[skill]++;
        fooey++;
        xpReq = calcXpReq(skillLevels[skill], 30.f);
    }
    return fooey;
}

unsigned short Player::getSkillLevel(SkillType skill) {
    return skillLevels[skill];
}

int Player::getSkillExpPercent(SkillType skill) {
    int xpReq = (int)(pow(skillLevels[skill] + 1, 1.1) * 30.);
    return skillExps[skill] * 100 / xpReq;
}

PrimeFolder* Player::getPrimeFolder() {
    return primeFolder;
}

//the spell index used by the player lacks the two least significant bits
int Player::getSpellLevel(int spellIndex) {
    int trueSpellIndex = spellIndex >> 2;
    map<int, playerSpell>::iterator itr = playerSpells.find(trueSpellIndex);
    if (itr == playerSpells.end()) return 0;
    else return itr->second.level;
}

void Player::trainSpell(int spellIndex, int xpGained) {
    int trueSpellIndex = spellIndex >> 2;
    map<int, playerSpell>::iterator itr = playerSpells.find(trueSpellIndex);
    playerSpell* pSpell;
    if (itr == playerSpells.end()) {
        pSpell = &playerSpells[trueSpellIndex];
        pSpell->level = 0;
        pSpell->exp = 0;
    } else {
        pSpell = &itr->second;
    }
    pSpell->exp += xpGained;
    int xpReq = calcXpReq(pSpell->level, 2.f);
    while (pSpell->exp >= xpReq) {
        pSpell->exp -= xpReq;
        pSpell->level++;
        xpReq = calcXpReq(pSpell->level, 2.f);
    }
}

const map<int, playerSpell>::iterator Player::getSpellsBegin() {
    return playerSpells.begin();
}
const map<int, playerSpell>::iterator Player::getSpellsEnd() {
    return playerSpells.end();
}

int Player::takeFromXpBank(int amount) {
    int rem = min(amount, xpBank + 10);
    xpBank -= rem;
    return rem;
}

void Player::bankXp(int amount) {
    xpBank += amount;
}
int Player::getXpBank() {
    return xpBank;
}
