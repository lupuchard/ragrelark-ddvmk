#include "Start.h"

void Start::castSpell(unsigned int spellI, Unit* unit, Zone* zone) {
    Ability* ability = getAbility(spellI);
    if (ability) {
        int level = 0;
        if (unit == player->getUnit()) {
            level += player->getSpellLevel(spellI);
            for (int i = 0; i < primeFolder->getEquips()->getNumItems(); i++) {
                if (getItemType(primeFolder->getEquips()->getItem(i)->itemType)->hasAbility(spellI)) {
                    level += 5;
                    if (level < 10) level = 10;
                    break;
                }
            }
        }
        if (level >= 10) {
            int mana = unit->getStatValue(S_MANA);
            if (mana >= ability->getCost()) {
                int netLevel = level + unit->getStatValue(S_AFF);
                int ch = netLevel - ability->getDifficulty();

                bool success = false;
                if (ch >= 10) success = true;
                else if (ch > 0) {
                    cout << "ch " << (int)ch << endl;
                    if (rand() % 10 < ch) success = true;
                } else {
                    addMessage("You are not skilled enough to cast that spell!", black);
                    return;
                }
                unit->modifyStat(S_MANA, -ability->getCost());
                int sp = 0;
                if (unit == player->getUnit()) {
                    sp = player->getSkillLevel(SKL_QCAST) / 4;
                }
                unit->theTime += actionTimePassed(ability->getTimeTake(), sp);
                Unit* target = NULL;
                if (success) {
                    //calc power
                    switch(spellI) {
                    case SPELL_LIGHT:
                        if (unit == player->getUnit()) addMessage("You shed some light on the situation.", black);
                        myFovCirclePerm(zone, unit->x, unit->y, 6, 1);
                        playerFieldOfView(true);
                        break;
                    default: if (spellI) addMessage("Hmm.", black); break;
                    }
                } else {
                    if (unit == player->getUnit()) addMessage("You miscast " + ability->getName() + ".", black);
                    else addMessage("The " + unit->name + " messes up.", black);
                }
                if (unit == player->getUnit()) {
                    int expGained = 0;

                    if (target) {
                        int expLeft = target->getStatValue(S_EXP);
                        if (expLeft) {
                            int expGained = min(expLeft, (int)target->getStatValue(S_LEVEL) + 1);
                            target->modifyStat(S_EXP, -expGained);
                        }
                        sapExp(unit, target, SKL_QCAST, 1);
                    } else {
                        expGained = player->takeFromXpBank(ability->getDifficulty() / 10);
                        debankExp(unit, SKL_QCAST, ability->getDifficulty() / 10);
                    }

                    int toteExp = player->getUnit()->modifyStat(S_EXP, expGained);
                    int lev = player->trainSpell(spellI, expGained);
                    if (lev) {
                        int level = player->getSpellLevel(spellI);
                        int leve = level / 10;
                        int evel = level % 10;
                        if (lev > 0) {
                            addMessage("Your ability to cast " + ability->getName() + " has increased to " + its(leve) + "." + its(evel) + "!", forest);
                        } else {
                            addMessage("Your ability to cast " + ability->getName() + " has increased to " + its(leve) + "." + its(evel) + "!", maroon);
                        }
                    }
                    int expReq = player->getUnit()->getStatValue(S_EXPREQ);
                    if (toteExp >= expReq) {
                        player->getUnit()->modifyStat(S_EXP, -expReq);
                        player->getUnit()->modifyStat(S_LEVEL, 1);
                    }
                    foon += expGained;
                    if (foon > 10) {
                        debankExp(unit, SKL_LEARN, foon / 10);
                        foon %= 10;
                    }
                }
            } else if (unit == player->getUnit()) {
                addMessage("You do not have enough mana to cast the spell '" + ability->getName() + "'.", black);
            }
        } else if (unit == player->getUnit()) {
            if (level) addMessage("You do not know that spell well enough yet to cast it without a spellbook.", black);
            else addMessage("You do not know that spell.", black);
        }
    } else {
        addMessage("That is not an existing spell.", black);
    }
}
