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
    skillExps = new int[Stat::getNumSkills()];
    skillLevels = new unsigned short[Stat::getNumSkills()];
    for (int i = 0; i < Stat::getNumSkills(); i++) {
        skillExps[i] = 0;
        skillLevels[i] = 0;
    }
    primeFolder = pFolder;
    unit = NULL;
    zone = NULL;
    area = NULL;
    currentZoneMemory = NULL;
    xpBank = 0;
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
    delete[] skillExps;
    delete[] skillLevels;
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

void Player::setName(String n) {
    name = n;
}

int Player::gainSkillExp(Skill* skill, int xpGained) {
    int i = skill->index;
    skillExps[i] += xpGained;
    if (skillExps[i] < 0) {
        if (skillLevels[i] >= 0) {
            int prevXpReq;
            if (skillLevels[i] == 1) prevXpReq = 100;
            else prevXpReq = calcXpReq(skillLevels[i] - 1, 30.f);
            skillExps[i] += prevXpReq;
            skillLevels[i]--;
            return -1;
        }
        return 0;
    }
    int xpReq = calcXpReq(skillLevels[i], 30.f);
    int fooey = 0;
    while (skillExps[i] >= xpReq) {
        skillExps[i] -= xpReq;
        skillLevels[i]++;
        fooey++;
        xpReq = calcXpReq(skillLevels[i], 30.f);
    }
    return fooey;
}

unsigned short Player::getSkillLevel(Skill* skill) {
    return skillLevels[skill->index];
}

int Player::getSkillExpPercent(Skill* skill) {
    int xpReq = (int)(pow(skillLevels[skill->index] + 1, 1.1) * 30.);
    return skillExps[skill->index] * 100 / xpReq;
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
    //xpGained = (int)(xpGained * (getSkillLevel(SKL_LEARN) / 10.f + 1.f));
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

void Player::save(std::ostream& saveData) {
    outStr(name, saveData);
    outPair(unit->pos, saveData);
    outPair(zone->dungeonTag(), saveData);
    outStr(area->getName(), saveData);
    primeFolder->save(saveData);

    outSht(memoryBank.size(), saveData);
    for (std::map<Zone*, ZoneMemory*>::iterator iter = memoryBank.begin(); iter != memoryBank.end(); ++iter) {
        Zone* z = iter->first;
        ZoneMemory* m = iter->second;
        outPair(zone->dungeonTag(), saveData);
        int i;
        for (i = 0; i < z->getWidth() * z->getHeight(); i++) {
            saveData.put(m->bottomLoc[i]); saveData.put(m->bottomTex[i]);
            saveData.put(m->topLoc[i]);    saveData.put(m->topTex[i]);
        }
    }

    outInt(xpBank, saveData);
    for (int i = 0; i < Stat::getNumSkills(); i++) {
        outInt(skillExps[i], saveData);
        outSht(skillLevels[i], saveData);
    }

    outSht(playerSpells.size(), saveData);
    for (std::map<int, PlayerSpell>::iterator iter = playerSpells.begin(); iter != playerSpells.end(); ++iter) {
        outSht(iter->first, saveData);
        outInt(iter->second.exp, saveData);
        outSht(iter->second.level, saveData);
    }
}

void Player::load(std::istream& saveData, World* world) {
    name = inStr(saveData);
    Coord unitCoord = Coord(inPair(saveData));
    std::pair<int, int> dungeonTag = inPair(saveData);
    String areaName;
    areaName = inStr(saveData);
    area = world->getArea(areaName);
    zone = area->getZone(dungeonTag);
    primeFolder->load(saveData);

    unsigned short memoryBankSize = inSht(saveData);
    for (unsigned int i = 0; i < memoryBankSize; i++) {
        Zone* z = area->getZone(inPair(saveData));
        int len = z->getWidth() * z->getHeight();
        ZoneMemory* m = new ZoneMemory;
        m->bottomTex = new unsigned char[len];
        m->bottomLoc = new unsigned char[len];
        m->topTex = new unsigned char[len];
        m->topLoc = new unsigned char[len];
        for (int i = 0; i < len; i++) {
            m->bottomLoc[i] = saveData.get(); m->bottomTex[i] = saveData.get();
            m->topLoc[i] = saveData.get(); m->topTex[i] = saveData.get();
        }
        memoryBank[z] = m;
    }

    xpBank = inInt(saveData);
    for (int i = 0; i < Stat::getNumSkills(); i++) {
        skillExps[i] = inInt(saveData);
        skillLevels[i] = inSht(saveData);
    }

    unsigned short numPlayerSpells = inSht(saveData);
    for (unsigned int i = 0; i < numPlayerSpells; i++) {
        short spell = inSht(saveData);
        int exp = inInt(saveData);
        unsigned short level = inSht(saveData);
        playerSpells[spell] = PlayerSpell{exp, level};
    }

    unit = zone->getLocationAt(unitCoord)->unit;
    currentZoneMemory = memoryBank[zone];
}
