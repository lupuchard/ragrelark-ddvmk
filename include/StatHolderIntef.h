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

enum VOwner{V_WORLD, V_ITEM, V_UNIT, V_TILE, V_ZONE, V_AREA, V_ENEMY};
enum VType{V_STAT, V_SKILL, V_CONDITION};
enum SkillType{SKL_ATHL, SKL_MELEE, SKL_UNARM, SKL_LIFT, SKL_FORT, SKL_RPOIS, SKL_RBURN, SKL_RCOLD, SKL_CHANN, SKL_QCAST, SKL_CONC, SKL_AFF4,
    SKL_LEARN, SKL_KNOW, SKL_INT3, SKL_INT4, SKL_SEARC, SKL_SIGHT, SKL_HEAR, SKL_FARSH, SKL_DODGE, SKL_RANGE, SKL_THROW, SKL_CRIT,
    SKL_BART, SKL_INTIM, SKL_CHARM, SKL_CHA4, SKL_ARMOR, SKL_SHLD, SKL_DEF3, SKL_DEF4,
    SKL_BASH, SKL_PIERC, SKL_SLASH, SKL_SPEC, SKL_WATER, SKL_FIRE, SKL_AIR, SKL_EARTH, SKL_NECRO, SKL_ILLUS, SKL_SUMM, SKL_ENCH,
    SKL_AXE, SKL_SWORD, SKL_SCYTH, SKL_HAMAC, SKL_STAFF, SKL_FLIP, SKL_BOW, SKL_CROSS, SKL_FIREM, SKL_SPEAR, SKL_DAGGE, SKL_TRID};

class StatHolderIntef {
    public:
        StatHolderIntef();
        virtual ~StatHolderIntef();
        virtual VOwner getOwner() = 0;
    protected:
    private:
};

#endif // STATHOLDERINTEF_H
