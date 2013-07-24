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

#ifndef PLAYER_H
#define PLAYER_H

#include "Area.h"
#include "PrimeFolder.h"
#include "Ability.h"

struct ZoneMemory {
    unsigned char* bottomTex;
    unsigned char* bottomLoc;
    unsigned char* topTex;
    unsigned char* topLoc;
};

struct PlayerSpell {
    int exp;
    unsigned short level;
};

class Player: boost::noncopyable {
    public:
        Player(PrimeFolder* primeFolder);
        virtual ~Player();

        Zone* getZone();
        Area* getArea();
        Unit* getUnit();
        PrimeFolder* getPrimeFolder();
        void setZone(Zone* z);
        void setArea(Area* a);
        void setUnitProto(StatHolder* p);
        void setName(String name);

        std::pair<int, int> getMemoryBottom(Coord c);
        std::pair<int, int> getMemoryTop(Coord c);
        void setMemory(Coord c, unsigned char bt, unsigned char bl, unsigned char tt, unsigned char tl);

        int gainSkillExp(Skill* skill, int xpGained); //returns the change in the skills level after the xp gain
        unsigned short getSkillLevel(Skill* skill);
        int getSkillExpPercent(Skill* skill);

        int getSpellLevel(int spellIndex);
        int trainSpell(int spellIndex, int xpGained);
        const std::map<int, PlayerSpell>::iterator getSpellsBegin();
        const std::map<int, PlayerSpell>::iterator getSpellsEnd();

        int takeFromXpBank(int amount);
        void bankXp(int amount);
        int getXpBank();
    private:
        String name;
        Unit* unit;
        Zone* zone;
        Area* area;
        PrimeFolder* primeFolder;
        std::map<Zone*, ZoneMemory*> memoryBank;
        ZoneMemory* currentZoneMemory;
        int* skillExps;
        unsigned short* skillLevels;
        std::map<int, PlayerSpell> playerSpells;
        int xpBank;
};

#endif // PLAYER_H
