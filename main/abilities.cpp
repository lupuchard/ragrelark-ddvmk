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

void Start::castSpell(unsigned int spellI, Unit* unit, Zone* zone) {
    Ability* ability = Ability::get(spellI);
    if (ability) {
        int level = 0;
        if (unit == player->getUnit()) {
            level += player->getSpellLevel(spellI);
            for (int i = 0; i < primeFolder->getEquips()->getNumItems(); i++) {
                if (ItemType::get(primeFolder->getEquips()->getItem(i)->itemType)->hasAbility(spellI)) {
                    level += 5;
                    if (level < 10) level = 10;
                    break;
                }
            }
        }
        if (level >= 10) {
            int mana = unit->getStatValue(Stat::MANA);
            if (mana >= ability->getCost()) {
                int netLevel = level + unit->getStatValue(Stat::AFF);
                int ch = netLevel - ability->getDifficulty();

                bool success = false;
                if (ch >= 10) success = true;
                else if (ch > 0) {
                    //cout << "ch " << (int)ch << endl;
                    if (rand() % 10 < ch) success = true;
                } else {
                    addMessage("You are not skilled enough to cast that spell!", BLACK);
                    return;
                }
                unit->modifyStat(Stat::MANA, -ability->getCost());
                int sp = 0;
                /*if (unit == player->getUnit()) {
                    sp = player->getSkillLevel(SKL_QCAST) / 4;
                }*/
                unit->theTime += actionTimePassed(ability->getTimeTake(), sp);
                Unit* target = NULL;
                if (success) {
                    //calc power
                    switch(spellI) {
                    case SPELL_LIGHT:
                        if (unit == player->getUnit()) addMessage("You shed some light on the situation.", BLACK);
                        myFovCirclePerm(zone, unit->pos, 6, 1);
                        playerFieldOfView(true);
                        break;
                    default: if (spellI) addMessage("Hmm.", BLACK); break;
                    }
                } else {
                    if (unit == player->getUnit()) addMessage("You miscast " + ability->getName() + ".", BLACK);
                    else addMessage("The " + unit->name + " messes up.", BLACK);
                }
                if (unit == player->getUnit()) {
                    int expGained = 0;

                    if (target) {
                        int expLeft = target->getStatValue(Stat::EXP);
                        if (expLeft) {
                            int expGained = std::min(expLeft, (int)target->getStatValue(Stat::LEVEL) + 1);
                            target->modifyStat(Stat::EXP, -expGained);
                        }
                        //sapExp(unit, target, SKL_QCAST, 1);
                    } else {
                        expGained = player->takeFromXpBank(ability->getDifficulty() / 10);
                        //debankExp(unit, SKL_QCAST, ability->getDifficulty() / 10);
                    }

                    int toteExp = player->getUnit()->modifyStat(Stat::EXP, expGained);
                    int lev = player->trainSpell(spellI, expGained);
                    if (lev) {
                        int level = player->getSpellLevel(spellI);
                        int leve = level / 10;
                        int evel = level % 10;
                        if (lev > 0) {
                            addMessage("Your ability to cast " + ability->getName() + " has increased to " + its(leve) + "." + its(evel) + "!", FOREST);
                        } else {
                            addMessage("Your ability to cast " + ability->getName() + " has increased to " + its(leve) + "." + its(evel) + "!", MAROON);
                        }
                    }
                    int expReq = player->getUnit()->getStatValue(Stat::EXPREQ);
                    if (toteExp >= expReq) {
                        player->getUnit()->modifyStat(Stat::EXP, -expReq);
                        player->getUnit()->modifyStat(Stat::LEVEL, 1);
                    }
                    foon += expGained;
                    if (foon > 10) {
                        //debankExp(unit, SKL_LEARN, foon / 10);
                        foon %= 10;
                    }
                }
            } else if (unit == player->getUnit()) {
                addMessage("You do not have enough mana to cast the spell '" + ability->getName() + "'.", BLACK);
            }
        } else if (unit == player->getUnit()) {
            if (level) addMessage("You do not know that spell well enough yet to cast it without a spellbook.", BLACK);
            else addMessage("You do not know that spell.", BLACK);
        }
    } else {
        addMessage("That is not an existing spell.", BLACK);
    }
}
