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
        interval0 += timePassed;
        interval1 += timePassed;
        interval2 += timePassed;
        interval3 += timePassed;
        int i0times = interval0 / INTERVAL_0_TIM;
        int i1times = interval1 / INTERVAL_1_TIM;
        int i2times = interval2 / INTERVAL_2_TIM;
        int i3times = interval3 / INTERVAL_3_TIM;

        if (i0times > 0) {
            int poisonLeft = pUnit->getStatValue(S_POISON);
            //cout << "happens1 " << poisonLeft << endl;
            if (poisonLeft > 0) {
                int poisoning = 0;
                if (pUnit->getCondition(8)) {
                    poisoning++;
                } if (pUnit->getCondition(9)) {
                    poisoning += 2;
                } if (pUnit->getCondition(10)) {
                    poisoning += 4;
                }
                //cout << "happens2 " << poisoning << endl;
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
            }
        }
        if (i1times > 0) {
            interval1 = interval1 % INTERVAL_1_TIM;

        }
        if (i2times > 0) {
            interval2 = interval2 % INTERVAL_2_TIM;
        }
        if (i3times > 0) {
            interval3 = interval3 % INTERVAL_3_TIM;
            pUnit->modifyStat(S_HUNGER, -(int)(pUnit->getStatValue(S_METABOLISM) * 1.7361111 * i3times)); //base metabolism is 100 calories per hour
            cout << "mets " << pUnit->getStatValue(S_METABOLISM) << endl;
        }
        cout << "mets " << pUnit->getStatValue(S_METABOLISM) << endl;
        //cout << "mets " << pUnit->getStatValue(S_HUNGER) << endl;
        pUnit->modifyStat(S_STAMINA, timePassed);
        world->theTime = pUnit->theTime;

        set<pair<Unit*, Zone*> >::iterator iter = areaUnits.begin();
        for (; iter != areaUnits.end(); iter++) {
            Unit* unit = iter->first;
            if (unit != player->getUnit()) {
                while (unit->theTime < world->theTime) {
                    int before = unit->theTime;
                    ai(unit, iter->second);
                    if (before == unit->theTime) {
                        unit->theTime++;
                        cout << "PROBLEMS AI NOT MOVING" << endl;
                    }
                }
            }
            for (int i = 0; i < i2times; i++) {
                unit->modifyStat(S_HP, (int)(unit->getStatValueF(S_HPREGEN) * unit->getStatValue(S_HP)));
                unit->modifyStat(S_MANA, (int)(unit->getStatValueF(S_MANAREGEN) * unit->getStatValue(S_MANA)));
            }
        }
    }
}
