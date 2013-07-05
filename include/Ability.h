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

#ifndef ABILITY_H
#define ABILITY_H

#include <string>

#define SPELL_LIGHT 768

class Ability {
    public:
        Ability(std::string name, int manaCost, int difficulty, int timeTake, int classI);

        std::string getName();
        int getCost();
        int getDifficulty();
        int getTimeTake();
        int getI();
    protected:
    private:
        std::string name;
        unsigned short manaCost;
        unsigned short difficulty;
        unsigned short classificationI;
        unsigned short timeTake;
};

void addAbility(Ability* ability);
Ability* getAbility(int index);
Ability* getAbility(bool wind, bool earth, bool water, bool fire, bool phys, bool ment, bool light, bool dark, int power);
void cleanAbilities();

#endif // ABILITY_H
