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

#ifndef STATHOLDERINTEF_H
#define STATHOLDERINTEF_H

enum VOwner{V_ITEM, V_UNIT, V_ZONE, V_ENEMY};
enum VType{V_STAT, V_SKILL};

class StatHolderIntef {
    public:
        virtual VOwner getOwner() = 0;
};

#endif // STATHOLDERINTEF_H
