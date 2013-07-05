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

#include "EnemyEquipStatHolder.h"

EnemyEquipStatHolder::EnemyEquipStatHolder(): StatHolder(V_ITEM), mobEquips(NULL) {}

short EnemyEquipStatHolder::getStatValue(int stat) {
    int val = 0;
    for (int i = 0; i < mobEquips->len; i++) {
        val += getItemType(mobEquips->equips[i].itemType)->getStatValue(stat);
    }
    return val;
}

float EnemyEquipStatHolder::getStatValueF(int stat) {
    float val = 0;
    for (int i = 0; i < mobEquips->len; i++) {
        val += getItemType(mobEquips->equips[i].itemType)->getStatValueF(stat);
    }
    return val;
}

void EnemyEquipStatHolder::setMobEquips(MobEquips* mEquips) {
    mobEquips = mEquips;
}
