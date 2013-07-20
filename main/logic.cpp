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

/*
metabolism!
base: 100/hour
-50% while sleeping
+25% regen health
+25% regen stamina
+50% regen mana
+50% >100% satiated "bloated"
-15% <40% satiated (also minor loss of stats) "very hungry"
-30% <15% satiated (also loss of stats)       "near starving"
-45% <5% satiated (also major loss of stats)  "starving"
*/

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
            /*int poisonLeft = pUnit->getStatValue(S_POISON);
            if (poisonLeft > 0) {
                int poisoning = 0;
                if (pUnit->getCondition(8)) {
                    poisoning++;
                } if (pUnit->getCondition(9)) {
                    poisoning += 2;
                } if (pUnit->getCondition(10)) {
                    poisoning += 4;
                }
                if (poisoning > 0) {
                    pUnit->modifyStat(S_HP, -poisoning);
                }
                pUnit->modifyStat(S_POISON, -1);
                if (poisonLeft == 1) {
                    //cycle through and disable all the poisons
                    for (int i = 0; i < 16; i++) {
                        pUnit->setCondition(8 + i, false);
                    }
                    removeStatus(ST_POIS);
                }
            }*/
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
                //if (unit->getStatValue(S_SWARM) && ((Swarmer*)unit)->howMany() >= 2) cout << "hmm " << unit->getStatValue(S_AI) << endl;
                //if (unit->getStatValue(S_SWARM)) cout << "hmm " << unit->getStatValue(S_AI) << endl;
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
