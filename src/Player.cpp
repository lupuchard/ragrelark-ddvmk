/*
 *  Copyright 2013 Luke Puchner-Hardman
 *
 *  This file is part of Ragrelark.
 *  Ragrelark is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ragrelark is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ragrelark.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    unit = NULL;
    zone = NULL;
    area = NULL;
    currentZoneMemory = NULL;
    xpBank = 0;
    //trainSpell(SPELL_LIGHT, 100);
}

Player::~Player() {
    //deletes all the players personal memories of how things were one day
    for (std::map<Zone*, ZoneMemory*>::iterator i = memoryBank.begin(); i != memoryBank.end(); ++i) {
        ZoneMemory* m = i->second;
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
    std::map<Zone*, ZoneMemory*>::iterator it = memoryBank.find(z);
    if (it == memoryBank.end()) {
        int len = z->getWidth() * z->getHeight();
        ZoneMemory* newZoneMemory = new ZoneMemory;
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

std::pair<int, int> Player::getMemoryTop(Coord pos) {
    int i = pos.index(zone->getWidth());
    return std::pair<int, int>(currentZoneMemory->topTex[i], currentZoneMemory->topLoc[i]);
}

std::pair<int, int> Player::getMemoryBottom(Coord pos) {
    int i = pos.index(zone->getWidth());
    return std::pair<int, int>(currentZoneMemory->bottomTex[i], currentZoneMemory->bottomLoc[i]);
}

void Player::setMemory(Coord pos, unsigned char bt, unsigned char bl, unsigned char tt, unsigned char tl) {
    int i = pos.index(zone->getWidth());
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

void Player::setName(std::string n) {
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
    std::map<int, PlayerSpell>::iterator itr = playerSpells.find(trueSpellIndex);
    if (itr == playerSpells.end()) return 0;
    else return itr->second.level;
}

int Player::trainSpell(int spellIndex, int xpGained) {
    xpGained = (int)(xpGained * (getSkillLevel(SKL_LEARN) / 10.f + 1.f));
    int trueSpellIndex = spellIndex >> 2;
    std::map<int, PlayerSpell>::iterator itr = playerSpells.find(trueSpellIndex);
    PlayerSpell* pSpell;
    if (itr == playerSpells.end()) {
        pSpell = &playerSpells[trueSpellIndex];
        pSpell->level = 0;
        pSpell->exp = 0;
    } else {
        pSpell = &itr->second;
    }
    pSpell->exp += xpGained;
    int xpReq = calcXpReq(pSpell->level, 2.f);
    int fooey = 0;
    while (pSpell->exp >= xpReq) {
        pSpell->exp -= xpReq;
        pSpell->level++;
        fooey++;
        xpReq = calcXpReq(pSpell->level, 2.f);
    }
    return fooey;
}

const std::map<int, PlayerSpell>::iterator Player::getSpellsBegin() {
    return playerSpells.begin();
}
const std::map<int, PlayerSpell>::iterator Player::getSpellsEnd() {
    return playerSpells.end();
}

int Player::takeFromXpBank(int amount) {
    int rem = std::min(amount, xpBank + 10);
    xpBank -= rem;
    return rem;
}

void Player::bankXp(int amount) {
    xpBank += amount;
}
int Player::getXpBank() {
    return xpBank;
}
