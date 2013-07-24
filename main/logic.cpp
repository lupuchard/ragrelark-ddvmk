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

#include "Start.h"

bool depoison(Unit* unit, int stat) {
    if (unit->getStatValue(stat) > 0) {
        return unit->modifyStat(stat, -1);
    }
    return false;
}

void Start::logic() {
    Unit* pUnit = player->getUnit();
    if (pUnit->theTime > world->theTime) {
        int timePassed = pUnit->theTime - world->theTime;
        for (unsigned int i = 0; i < 4; i++) intervals[i] += timePassed;
        int i0times = intervals[0] / INTERVAL_0_TIM;
        int i1times = intervals[1] / INTERVAL_1_TIM;
        int i2times = intervals[2] / INTERVAL_2_TIM;
        int i3times = intervals[3] / INTERVAL_3_TIM;

        if (i0times > 0) {
            bool poisoned = false;
            poisoned |= depoison(pUnit, Stat::POIS_PHYS);
            poisoned |= depoison(pUnit, Stat::POIS_MENT);
            poisoned |= depoison(pUnit, Stat::POIS_REGEN);
            poisoned |= depoison(pUnit, Stat::POIS_EXTRA);
            if (!poisoned) {
                removeStatus(ST_POIS);
            }
        }
        if (i1times > 0) {
            intervals[1] %= INTERVAL_1_TIM;
        }
        if (i2times > 0) {
            intervals[2] %= INTERVAL_2_TIM;
        }
        if (i3times > 0) {
            intervals[3] %= INTERVAL_3_TIM;
            int hung = pUnit->modifyStat(Stat::HUNGER, -(int)(pUnit->getStatValue(Stat::METABOLISM) * 1.7361111 * i3times)); //base metabolism is 100 calories per hour
            if (hung < MAX_HUNGER * .05f) {
                addStatus("starving", RED, ST_HUNG);
            } else if (hung < MAX_HUNGER * .15) {
                addStatus("near starving", MAROON, ST_HUNG);
            } else if (hung < MAX_HUNGER * .4) {
                addStatus("very hungry", SALMON, ST_HUNG);
            } else if (hung <= MAX_HUNGER) {
                removeStatus(ST_HUNG);
            }
        }
        pUnit->modifyStat(Stat::STAMINA, timePassed);
        world->theTime = pUnit->theTime;

        std::set<std::pair<Unit*, Zone*> >::iterator iter = areaUnits.begin();
        for (; iter != areaUnits.end(); ++iter) {
            Unit* unit = iter->first;
            if (unit != player->getUnit()) {
                while (unit->theTime < world->theTime) {
                    int before = unit->theTime;
                    ai(unit, iter->second);
                    if (before == unit->theTime) {
                        unit->theTime += 3;
                        //cout << "PROBLEMS AI NOT MOVING" << endl;
                    }
                }
            }
            for (int i = 0; i < i2times; i++) {
                double foon1 = rand() / (double)RAND_MAX;
                float hpr = unit->getStatValueF(Stat::HPREGEN) * unit->getStatValue(Stat::MAXHP) / 10.f;
                int hpa = (int)hpr;
                hpr = fmodf(hpr, 1.f);
                if (foon1 < hpr) hpa++;
                if (hpa) unit->modifyStat(Stat::HP, hpa);

                double foon2 = rand() / (double)RAND_MAX;
                int maxMana = unit->getStatValue(Stat::MAXMANA);
                float manar = unit->getStatValueF(Stat::MANAREGEN) * maxMana / 10.f;
                int manaa = (int)manar;
                manar = fmodf(manar, 1.f);
                if (foon2 < manar) manaa++;
                if (manaa) {
                    int mana = unit->getStatValue(Stat::MANA);
                    if (mana != maxMana) {
                        //debankExp(unit, SKL_CHANN, std::min(manaa, maxMana - mana));
                        unit->modifyStat(Stat::MANA, manaa);
                    }
                }
            }
        }
    }
}
