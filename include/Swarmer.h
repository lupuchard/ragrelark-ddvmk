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

#ifndef SWARMER_H
#define SWARMER_H

#include "Unit.h"

class Swarmer: public Unit {
    public:
        Swarmer(std::string n, StatHolder* prototype);
        virtual ~Swarmer();

        short getStatValue(int stat);
        short modifyStat(int stat, int amount);
        float getStatValueF(int stat);
        float modifyStatF(int stat, float amount);

        int howMany();

        void add(Swarmer* unit);
        Unit* take(int unit);
    private:
        std::vector<Swarmer*> others;
};

#endif // SWARM_H
