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

#include <math.h>
#include "Unit.h"
#include "Zone.h"
#include "Start.h"
#include "Area.h"

//one tick is one-fourth of a second
#define T_WALK_ST 10
#define T_WALK_DI 14
#define T_OPENDOOR 15
#define T_ATTACK 20
#define T_EAT 40
#define T_ROCK 30

int Start::actionTimePassed(int time, int speed) {
    return std::max((int)(time * std::pow(.95, speed - 8)), 1);
}

double Start::chanceHit(double m) {
    return 1 / (std::pow(E, (m / -2.)) * 3 + 1);
    //1 E e-evasion u-accuracy - 24 + $crit 200 / - -2 / ^ 3 * 1 + /
}

double Start::defDam(double preDam, int defense) {
    return preDam * std::pow(.98, defense);
}

//this method runs to tell the unit in the zone what to do, it chooses it's action
void Start::ai(Unit* unit, Zone* zone) {
    int ai = unit->getStatValue(S_AI); //the type of AI the monster uses, currently only two.
    bool inZone = zone == player->getZone(); //whether the player is in this zone or not. monsters outside the zone will always just sit still
    Unit* target = player->getUnit();
    switch(ai) {
        case AI_STILL: unit->theTime += 10; break;//just stands still
        case AI_HOSTILE: {
            if (inZone && unit->pointOnPath == -1) { //this means that a path does not exist so one needs to be created first
                if (visibilities[unit->pos.index(zone->getWidth())]) {
                    makePath(unit, target->pos, zone, P_NORMAL);
                    if (unit->pointOnPath == -1) {
                        makePath(unit, target->pos, zone, P_PASSUNITS);
                    }
                }
            }
            if (unit->pointOnPath > -1) { //it performs the check again because a path may have failed to be created (player inaccessable or something)
                followPath(unit, zone);
            } else {
                unit->theTime += 20;
            }
        } break;
        case AI_HOSTILESMART: unit->theTime += 10; break;
        case AI_PASSIVE: {
            if (inZone) {
                if (unit->pointOnPath > -1) {
                    followPath(unit, zone);
                } else {
                    int foon = rand() % 20;
                    if (foon >= 10) unit->theTime += 100;
                    else {
                        Coord t = unit->pos + DIRS[foon];
                        if (t.x >= 0 && t.y >= 0 && t.x < zone->getWidth() && t.y < zone->getHeight() && !zone->getLocationAt(t)->hasUnit()) {
                            moveUnit(unit, zone, foon);
                        } else {
                            unit->theTime += 10;
                        }
                    }
                }
            }
        } break;
        default: unit->theTime += 10; break;
    }
}

void Start::followPath(Unit* unit, Zone* zone) {
    Path* thePath = unit->currentPath;
    unit->pointOnPath++;
    if (thePath->pathLocs[unit->pointOnPath].x == -P_STAIRS) {
        goTheStairs(unit, zone);
    } else {
        int dir = 1;
        Coord c = thePath->pathLocs[unit->pointOnPath] - unit->pos;
        for (; dir <= 9; dir++) { //this loop is used to determine the direction so that moveUnit needs one
            if (DIRS[dir] == c) break;
        }
        if (dir < 10) {
            moveUnit(unit, zone, dir);
        } else {
            unit->pointOnPath = -1;
            unit->theTime += 20;
            //cout << "mob move mistake " << dir << " " << cx << " " << cy << " " << unit->x << " " << unit->y << endl;
        }
    }
}

void Start::playerWalkStaminaDrain(int* movSpeed, int tim, Unit* unit) {
    if (unit == player->getUnit()) {
        switch(loadStatus) {
            case 0: unit->modifyStat(S_STAMINA, -tim / 2); break;
            case 1:
                *movSpeed -= 1;
                unit->modifyStat(S_STAMINA, -tim);
                debankExp(unit, SKL_LIFT, 1);
                break;
            case 2:
                *movSpeed -= 2;
                unit->modifyStat(S_STAMINA, -tim * 3 / 2);
                debankExp(unit, SKL_LIFT, 2);
                break;
            case 3:
                *movSpeed -= 3;
                unit->modifyStat(S_STAMINA, -tim * 2);
                debankExp(unit, SKL_LIFT, 3);
                break;
            default:
                *movSpeed -= 4;
                unit->modifyStat(S_STAMINA, -tim * 5 / 2);
                debankExp(unit, SKL_LIFT, 4);
                break;
        }
        int stam = unit->getStatValue(S_STAMINA);
        if (stam < 2000) {
            *movSpeed -= 2;
        } else if (stam < 5000) {
            *movSpeed -= 1;
        }
    }
}

/* Moves a unit in a zone in a direction!!???!? */
void Start::moveUnit(Unit* unit, Zone* zone, int dir) {
    bool cond = unit->hasAnyConditions();
    if (cond && unit->getCondition(C_CONFUSION)) {
        int newDir = rand() % 11;
        if (newDir == 10) {
            unit->setCondition(C_CONFUSION, false);
            if (unit == player->getUnit()) {
                addMessage("You snap out of confusion.", BLACK);
                removeStatus(ST_CONF);
            }
        } else if (newDir) dir = newDir;
    }
    if (!(unit == player->getUnit() && loadStatus == 4)) {
        Coord prevPos = unit->pos;
        Coord newPos = prevPos + DIRS[dir];
        bool normalMove = false;
        bool diags = dir % 2;
        if (newPos.x >= 0 && newPos.y >= 0 && newPos.x < zone->getWidth() && newPos.y < zone->getHeight()) {
            Location* nextLoc = zone->getLocationAt(newPos);
            if (nextLoc->hasUnit()) {
                int swarm = unit->getStatValue(S_SWARM);
                if (swarm && nextLoc->unit->getProto() == unit->getProto()) {
                    Swarmer* unitS = static_cast<Swarmer*>(unit);
                    Swarmer* nextS = static_cast<Swarmer*>(nextLoc->unit);
                    if (unitS->howMany() + nextS->howMany() <= swarm) {
                        nextS->add(unitS);
                        raga.rMoveLoc(unit, prevPos, newPos);
                        zone->getLocationAt(prevPos)->removeUnit();
                        areaUnits.erase(areaUnits.find(std::pair<Unit*, Zone*>(unitS, zone)));
                        return;
                    } else {
                        unit->theTime += 2;
                        makePath(unit, player->getUnit()->pos, zone, P_NORMAL);
                    }
                } else if (unit == player->getUnit() || nextLoc->unit == player->getUnit()) {
                    strikeUnit(unit, zone, dir, true);
                } else {
                    unit->theTime += 2;
                    makePath(unit, player->getUnit()->pos, zone, P_NORMAL);
                }
                if (unit->pointOnPath > 0) {
                    unit->pointOnPath--;
                }
            } else {
                Location* prevLoc = zone->getLocationAt(unit->pos);
                int prevHeight = prevLoc->getTotalHeight();
                int height = nextLoc->getTotalHeight();
                if (nextLoc->isOpen() && !getTile(nextLoc->tile)->blocksMove()) {
                    int movSpeed = unit->getStatValue(S_MOVESPEED);
                    normalMove = true;
                    if (prevHeight != height && !unit->getStatValue(S_FLY)) {
                        if (fabs(prevHeight - height) <= 2) {
                            if (prevHeight < height) movSpeed--;
                        } else {
                            if (prevHeight < height && nextLoc->structure == S_ROCK) {
                                pushRock(unit, zone, dir);
                            } else {
                                openDoor(unit, zone, dir, true);
                            }
                            normalMove = false;
                        }
                    }
                    if (normalMove) {
                        changeLoc(unit, zone, newPos);
                        int tim;
                        if (diags) tim = T_WALK_DI;
                        else tim = T_WALK_ST;
                        if (unit == player->getUnit()) {
                            playerWalkStaminaDrain(&movSpeed, tim, unit);
                        }
                        //cout << unit->getStatValue(S_BMOVESPEED) << " what " << movSpeed << " " << tim << endl;
                        tim = actionTimePassed(tim, movSpeed);
                        //cout << "whaat " << tim << endl;
                        unit->theTime += tim;
                    }
                } else {
                    openDoor(unit, zone, dir, true);
                }
            }
        } else if (!diags) {
            SpeLoc sl = player->getArea()->moveZones(zone, newPos);
            if (sl.loc.x != -1) {
                if (unit == player->getUnit()) {
                    for (std::set<std::pair<Unit*, Zone*> >::iterator i = areaUnits.begin(); i != areaUnits.end(); ++i) {
                        Zone* z = i->second;
                        if (z == zone) {
                            Unit* u = i->first;
                            int ai = u->getStatValue(S_AI);
                            switch(ai) {
                            case 1: if (visibilities[u->pos.index(zone->getWidth())]) {
                                //cout << "happen " << newX << " " << newY << endl;
                                makePath(u, newPos, zone, P_NORMAL);
                                } break;
                            default: break;
                            }
                        }
                    }
                }
                changeLocZ(unit, zone, sl.z, sl.loc);
            }
        }
        if (normalMove && zone == player->getZone()) {
            raga.rMoveDir(unit, dir, prevPos);
        }
    }
}

void Start::pushRock(Unit* unit, Zone* zone, int dir) {
    Coord pathC = unit->pos + DIRS[dir] * 2;
    bool diags = dir % 2;
    if (pathC.inBounds(zone->getWidth(), zone->getHeight())) {
        Location* rockLoc = zone->getLocationAt(unit->pos + DIRS[dir]);
        if (rockLoc->structure == S_ROCK && fabs(rockLoc->height - zone->getLocationAt(unit->pos)->height) <= 2) {
            Location* pathLoc = zone->getLocationAt(pathC);
            int diff = pathLoc->height - rockLoc->height;
            if (diff == 2) {
                addMessage("It is too difficult to push the rock up this hill.", GRAY);
            } else if (diff <= 1) {
                if (pathLoc->hasUnit() || pathLoc->structure != S_NONE) {
                    addMessage("There is something in the way of the rock's path.", GRAY);
                } else {
                    rockLoc->structure = S_NONE;
                    pathLoc->structure = S_ROCK;
                    int tim;
                    if (diags) tim = T_WALK_DI;
                    else tim = T_WALK_ST;
                    int movSpeed = unit->getStatValue(S_MOVESPEED);
                    if (unit == player->getUnit()) {
                        playerWalkStaminaDrain(&movSpeed, tim, unit);
                    }
                    tim = actionTimePassed(tim, movSpeed);
                    unit->theTime += T_ROCK + diff + tim;
                    raga.rMoveDir(unit, dir, unit->pos);
                    changeLoc(unit, zone, unit->pos + DIRS[dir]);
                }
            }
        }
    }
}

void Start::goTheStairs(Unit* unit, Zone* zone) {
    SpeLoc sl = player->getArea()->moveConnection(zone, unit->pos);
    if (sl.loc.x != -1) {
        if (unit == player->getUnit()) {
            for (std::set<std::pair<Unit*, Zone*> >::iterator i = areaUnits.begin(); i != areaUnits.end(); ++i) {
                Zone* z = i->second;
                if (z == zone) {
                    Unit* u = i->first;
                    int ai = u->getStatValue(S_AI);
                    switch(ai) {
                    case 1: if (visibilities[u->pos.index(zone->getWidth())]) {
                        makePath(u, unit->pos, zone, P_STAIRS);
                        } break;
                    default: break;
                    }
                }
            }
        } else {
            addMessage("A " + unit->name + " comes from the stairs!", OLIVE);
        }
        changeLocZ(unit, zone, sl.z, sl.loc);
    }
}

void Start::hitCMod(Unit* defender, float& damage, float accuracy, float crit, int& hitType, battleSummary& sum) {
    float howLuckyAreYou = (float)rand() / RAND_MAX;
    int fiz = defender->getStatValue(S_EVA) - accuracy;
    if (howLuckyAreYou > chanceHit(fiz + 24 - crit / 2.f)) {
        damage *= 5;
        hitType = 4;
        sum.criticality = 4;
    } else if (howLuckyAreYou > chanceHit(fiz + 10 - crit)) {
        damage *= 2;
        hitType = 3;
        sum.criticality = 3;
    } else if (howLuckyAreYou > chanceHit(fiz)) {
        hitType = 2;
        sum.criticality = 2;
    } else if (howLuckyAreYou > chanceHit(fiz - 2)) {
        damage /= 2;
        hitType = 1;
        sum.dodge = true;
        sum.criticality = 1;
    } else {
        damage = 0;
        hitType = 0;
        sum.dodge = true;
        sum.criticality = 0;
    }
}

std::string Start::defenderNoun(Unit* attacker, Unit* defender) {
    if (defender == player->getUnit()) {
        if (defender == attacker) return "yourself";
        else return "you";
    } else {
        if (defender == attacker) return "itself";
        else return "the " + defender->name;
    }
}

void Start::hitSapping(Unit* attacker, Unit* defender, int criticality) {
    switch(criticality) {
        case 4: sapExp(attacker, defender, SKL_CRIT, 3); break;
        case 3: sapExp(attacker, defender, SKL_CRIT, 1); break;
        case 1: sapExp(attacker, defender, SKL_CRIT, 1);
                sapExp(defender, attacker, SKL_DODGE, 1); break;
        case 0: sapExp(defender, attacker, SKL_DODGE, 1); break;
        default: break;
    }
}

Color critColors[] = {Color{95, 31, 31, 0}, Color{63, 0, 0, 0}, MAROON, BRICK, RED};
std::string hitWord(int criticality, int attackType) {
    switch(criticality) {
        case 4:
        switch(attackType) {
            case WEAP_CLAWS: return "rend"; break;
            case WEAP_SPEAR: return "impale"; break;
            default: return "megacrit"; break;
        } break;
        case 3:
        switch(attackType) {
            case WEAP_FISTS: return "kick"; break;
            case WEAP_TENTACLE: return "tentacle slap"; break;
            case WEAP_SLAM: return "slam"; break;
            case WEAP_BITE: return "chomp"; break;
            case WEAP_CLUB: return "clobber"; break;
            case WEAP_DAGGER: return "stab"; break;
            case WEAP_AXE: return "chop"; break;
            default: return "crit"; break;
        } break;
        case 2:
        switch(attackType) {
            case WEAP_CLAWS: return "claw"; break;
            case WEAP_FISTS: return "punch"; break;
            case WEAP_HEAD: return "headbutt"; break;
            case WEAP_BITE: return "bite"; break;
            case WEAP_CLUB: return "smack"; break;
            case WEAP_AXE: return "axe"; break;
            case WEAP_SPEAR: return "jab"; break;
            case WEAP_SCYTHE: return "slash"; break;
            default: return "hit"; break;
        } break;
        case 1:
        switch(attackType) {
            case WEAP_CLAWS: return "scratch"; break;
            case WEAP_TENTACLE: return "rub"; break;
            case WEAP_FISTS: return "pap"; break;
            case WEAP_CLUB: return "tap"; break;
            case WEAP_SPEAR: return "poke"; break;
            default: return "scrape"; break;
        } break;
    }
    return "miss";
}
void Start::strikeUnit(Unit* unit, Zone* zone, int dir, bool safe) {
    if (unit == player->getUnit() && unit->getStatValue(S_STAMINA) < 2000) {
        addMessage("You are too exausted to attack!", GRAY);
    } else {
        Coord newPos = unit->pos + DIRS[dir];
        if (safe || (newPos.inBounds(zone->getWidth(), zone->getHeight()))) {
            Location* loc = zone->getLocationAt(newPos);
            if (loc->hasUnit()) {
                unit->setEnemy(loc->unit);
                Unit* defender = loc->unit;

                int weapType = 0;
                bool unarmed = true;
                if (unit == player->getUnit()) {
                    Item* item = primeFolder->getEquips()->getItem(E_RHAND);
                    ItemType* itemType = getItemType(item->itemType);
                    if (itemType->getType() != I_SLOT && item) {
                        if (!TYPE_RANGES[item->itemType]) {
                            weapType = itemType->getStatValue(S_DTYPE);
                            unarmed = false;
                        }
                    }
                } else if (unit->equipment) {
                    for (int i = 0; i < unit->equipment->len; i++) {
                        Item item = unit->equipment->equips[i];
                        ItemType* itemType = getItemType(item.itemType);
                        int slot = TYPE_SLOTS[itemType->getType()];
                        if (slot == E_RHAND || slot == E_BHANDS || slot == E_BBHANDS) {
                            weapType = itemType->getStatValue(S_DTYPE);
                            unarmed = false;
                            break;
                        }
                    }
                }
                unit->theTime += actionTimePassed(T_ATTACK, unit->getStatValue(S_ATTACKSPEED));
                battleSummary bs;
                float sk = 0;
                if (unit == player->getUnit()) {
                    sk = player->getSkillLevel(SKL_CRIT) / 100.f;
                    unit->modifyStat(S_STAMINA, -T_ATTACK);
                }

                int attackType;
                if (unarmed) attackType = unit->getStatValue(S_UNARMED);
                else attackType = weapType;
                std::string u1name;
                std::string u2name = defenderNoun(unit, defender);

                bs = attackUnit(unit->getStatValue(S_UNARMDAMAGE), unit->getStatValue(S_ACC), sk, attackType, defender, zone, dir);

                Color c = critColors[bs.criticality];
                std::string verb = hitWord(bs.criticality, attackType);
                std::string extra;
                if (unit == player->getUnit()) {
                    u1name = "you";
                } else {
                    u1name = "the " + unit->name;
                    c.green += c.red / 2;
                    verb = pluralize(verb);
                }

                if (!bs.killed && bs.hit) {
                    if (unarmed) sapExp(unit, defender, SKL_UNARM, 1);
                    else sapExp(unit, defender, SKL_MELEE, 1);
                    sapExp(defender, unit, SKL_FORT, 1);
                    int conditionAffectC = unit->getStatValue(S_ATTCONDCHANCE);
                    if (conditionAffectC > 0 && rand() % 100 <= conditionAffectC) {
                        int conditionAffect = unit->getStatValue(S_ATTCOND);
                        if (defender == player->getUnit()) {
                            switch(conditionAffect) {
                                case C_CONFUSION: extra = " and confuses";
                                addStatus("Confused", MAROON, ST_CONF);
                                break;
                                case C_POISON: extra = " and stings"; break;
                                default: break;
                            }
                        }
                        if (conditionAffect >= C_POISON && conditionAffect < C_POISON + 16) {
                            applyPoison(conditionAffect, 10, defender, unit);
                        } else {
                            defender->setCondition(conditionAffect, true);
                        }
                    }
                }
                addMessage(capitalize(u1name + " " + verb + " " + u2name + extra + "."), c);
            }
        }
    }
}

const int damStats[] = {S_MELDAMAGE, S_RANDAMAGE};
const SkillType damSkills[] = {SKL_MELEE, SKL_RANGE};
battleSummary Start::attackUnit(int power, float accuracy, float crit, int weapType, Unit* defender, Zone* zone, int dir) {
    battleSummary sum = {false, false, false, false};

    float damage = defDam(power, defender->getStatValue(S_DEFENSE));
    damage *= ((float)rand() / RAND_MAX) / 8 + .9375;
    int hitType;

    hitCMod(defender, damage, accuracy, crit, hitType, sum);

    raga.rAttack(defender->pos, dir, WEAP_DAM_TYPES[weapType], hitType);

    if (damage) {
        sum.hit = true;
        int hp = defender->modifyStat(S_HP, -(int)damage);
        int splatterChance;
        if (hp <= 0) {
            splatterChance = 10;
        } else {
            splatterChance = 10 * damage / hp;
        }
        splatterChance *= defender->getStatValue(S_SPLATTER);
        if (rand() % 200 < splatterChance) {
            makeSplatter(defender, zone, defender->pos);
        }

        if (hp <= 0) {
            if (defender != player->getUnit()) player->bankXp(defender->getStatValue(S_EXP));
            killUnit(defender, zone);
            sum.killed = true;
        } else {
            reactToAttack(defender, dir, zone);
        }
    }
    return sum;
}

void Start::shootUnit(Unit* attacker, Unit* defender, Zone* zone) {
    if (attacker == player->getUnit() && attacker->getStatValue(S_STAMINA) < 1500) {
        addMessage("You are too exausted to shoot!", GRAY);
    } else {
        attacker->setEnemy(defender);
        attacker->theTime += actionTimePassed(T_ATTACK, attacker->getStatValue(S_ATTACKSPEED));
        float sk = 0;
        if (attacker == player->getUnit()) {
            sk = player->getSkillLevel(SKL_CRIT) / 100.f;
            attacker->modifyStat(S_STAMINA, -(T_ATTACK / 2));
        }
        battleSummary bs;
        bs = attackUnit(attacker->getStatValue(S_RANDAMAGE), attacker->getStatValue(S_ACC), sk, WEAP_ARROW, defender, zone, 0);
        if (bs.hit) {
            sapExp(attacker, defender, SKL_RANGE, 1);
            sapExp(defender, attacker, SKL_FORT, 1);
        }
        Color c = critColors[bs.criticality];
        std::string verb = hitWord(bs.criticality, WEAP_ARROW);
        std::string u1name;
        std::string u2name = defenderNoun(attacker, defender);
        if (attacker == player->getUnit()) {
            u1name = "you fire and";
        } else {
            u1name = "the " + attacker->name + " shoots and";
            c.green += c.red / 2;
            verb = pluralize(verb);
        }
        addMessage(capitalize(u1name + " " + verb + " " + u2name + "."), c);
    }
}

void Start::projectItem(Item item, int power, int accuracy, Zone* zone, Coord to, Coord from) {
    std::string verb, u1name, u2name;

    ItemType* itemType = getItemType(item.itemType);

    int dam = itemType->getStatValue(S_IDAMAGE);
    int thro = itemType->getStatValue(S_THRO);
    int damty = itemType->getStatValue(S_DTYPE);
    int weight = itemType->getStatValue(S_WEIGHT);

    int damage = dam + (weight * power) / 100 + thro;

    if (damty) damty = WEAP_OBJ;
    Location* loc = zone->getLocationAt(to);
    if (loc->unit) {
        attackUnit(damage, accuracy, 0, damty, loc->unit, zone, 0);
    }
}

void Start::reactToAttack(Unit* unit, int dir, Zone* zone) {
    int ai = unit->getStatValue(S_AI);
    switch(ai) {
        case AI_PASSIVE: {
            makePath(unit, unit->pos - DIRS[dir], zone, P_FLEE);
            unit->theTime = world->theTime - T_WALK_ST;
            } break;
        default: break;
    }
}

int poisonWeights[] = {1, 2, 4, 1, 1, 1, 1, 1, 1, 3, 3, 1, 2, 2, 3, 3};
void Start::applyPoison(int condition, int duration, Unit* unit, Unit* poisoner) {
    int r = rand() % 100;
    int res = unit->getStatValue(S_RESPOIS);
    if (r < res / 5) {
        if (unit == player->getUnit()) addMessage("You resist the poison!", BLACK);
        return;
    } else if (r < res) {
        if (unit == player->getUnit()) addMessage("You slightly resist the poison.", BLACK);
        duration /= 2;
    }

    if (poisoner) {
        sapExp(unit, poisoner, SKL_RPOIS, 1);
    } else {
        debankExp(unit, SKL_RPOIS, poisonWeights[condition - 8]);
    }

    bool exists = false;

    bool slow = false;
    bool cripple = false;
    bool paralysis = false;
    bool deathly = false;
    bool befuddle = false;

    int weightBefore = 0;
    for (int i = 8; i < 24; i++) {
        if (unit->getCondition(i)) {
            if (i == condition) {
                exists = true;
            }

            weightBefore += poisonWeights[i - 8];
        }
        if (i == condition) {
            if (i >= 22) paralysis = true;
            else if (i >= 20) slow = true;
            else if (i == 10) deathly = true;
            else if (i >= 11 && i <= 14) cripple = true;
            else if (i >= 15 && i <= 18) befuddle = true;
        }
    }
    int currentDuration = unit->getStatValue(S_POISON);
    int prevPower = currentDuration * weightBefore;
    int addedWeight = poisonWeights[condition - 8];
    int addedPower = addedWeight * duration;

    if (exists) {
        if (addedPower > prevPower) {
            unit->setStat(S_POISON, addedPower / weightBefore);
        }
    } else {
        unit->setCondition(condition, true);
        weightBefore += addedWeight;
        unit->setStat(S_POISON, (prevPower + addedPower) / weightBefore);
    }
    if (unit == player->getUnit()) {
        //cout << "happens? " << endl;
        std::string statusString = "Poison";
        if (weightBefore >= 2) {
            statusString += " " + toRomanNumerals(weightBefore);
        }
        if (slow || cripple || paralysis || deathly || befuddle) {
            statusString += " (";
            bool start = false;
            if (slow) {
                statusString += "slow";
                start = true;
            }
            if (cripple) {
                if (start) statusString += ", ";
                statusString += "cripple";
                start = true;
            }
            if (befuddle) {
                if (start) statusString += ", ";
                statusString += "befuddle";
                start = true;
            }
            if (paralysis) {
                if (start) statusString += ", ";
                statusString += "paralyze";
                start = true;
            }
            if (deathly) {
                if (start) statusString += ", ";
                statusString += "toxic";
                start = true;
            }
            statusString += ")";
        }
        addStatus(statusString, PURPLE.darken(), ST_POIS);
    }
}

void Start::makeSplatter(Unit* unit, Zone* zone, Coord pos) {
    Location* loc = zone->getLocationAt(pos);
    int bloodColor = unit->getStatValue(S_BLOOD);
    if (loc->debris1 > bloodColor * 32 && loc->debris1 < (bloodColor + 1) * 32) {
        loc->debris1++;
    } else if (loc->debris2 > bloodColor * 32 && loc->debris2 < (bloodColor + 1) * 32) {
        loc->debris2++;
    } else {
        loc->debris2 = loc->debris1;
        loc->debris1 = bloodColor * 32 + 1;
    }
}

void Start::killUnit(Unit* unit, Zone* zone) {
    Location* locOfDeath = zone->getLocationAt(unit->pos);
    locOfDeath->removeUnit();
    areaUnits.erase(std::pair<Unit*, Zone*>(unit, zone));
    if (unit->equipment) {
        for (int i = 0; i < unit->equipment->len; i++) {
            locOfDeath->addItem(unit->equipment->equips[i]);
        }
    }
    int split = unit->getStatValue(S_SPLIT);
    if (split) {
        for (int i = 0; i < split; i++) {
            Unit* newUnit = mobSpawner->spawnMobSpeTag(unit->getStatValue(S_SPAWN), zone, unit->pos, world->theTime);
            if (newUnit) {
                areaUnits.insert(std::pair<Unit*, Zone*>(newUnit, zone));
                newUnit->theTime = world->theTime;
                raga.rMoveLoc(newUnit, unit->pos, newUnit->pos);
            } else break;
        }
    }
    unit->pos = Coord(-2, -2);
    if (unit == player->getUnit()) {
        addMessage("GAME OVER HAHA", BLACK);
    }
    if (unit->g.border) unitDeleteList.push_back(unit);
    else delete unit;
}

void Start::openDoor(Unit* unit, Zone* zone, int dir, bool safe) {
    Coord doorPos = unit->pos + DIRS[dir];
    if (unit->getStatValue(S_AI) > 1 && (safe || doorPos.inBounds(zone->getWidth(), zone->getHeight()))) {
        Location* nextLoc = zone->getLocationAt(doorPos);
        int str = nextLoc->structure;
        if (isClosedDoor(str)) {
            if (str == S_FOUNDDOOR) nextLoc->structure = S_WOODDOOR_OPEN;
            else nextLoc->structure = str + 1;
            if (str == S_STONEDOOR) unit->theTime += T_OPENDOOR * 2;
            else unit->theTime += T_OPENDOOR;
            if (unit == player->getUnit()) {
                addMessage("You open the door.", BLACK);
            } else {
                addMessage("A " + unit->name + " opens a door.", OLIVE);
            }
            if (unit == player->getUnit()) {
                playerFieldOfView(false);
            }
        }
    }
}

void Start::closeDoor(Unit* unit, Zone* zone, int dir, bool safe) {
    Coord doorPos = unit->pos + DIRS[dir];
    if (safe || doorPos.inBounds(zone->getWidth(), zone->getHeight())) {
        Location* nextLoc = zone->getLocationAt(doorPos);
        int str = nextLoc->structure;
        if (isOpenDoor(str)) {
            if (nextLoc->hasUnit()) {
                addMessage("There's something in the way!", GRAY);
            } else {
                nextLoc->structure = str - 1;
                if (str == S_STONEDOOR) unit->theTime += T_OPENDOOR * 2;
                else unit->theTime += T_OPENDOOR;
                if (unit == player->getUnit()) {
                    addMessage("You close the door.", BLACK);
                } else {
                    addMessage("A " + unit->name + " closes a door.", OLIVE);
                }
            }
            if (unit == player->getUnit()) {
                playerFieldOfView(false);
            }
        }
    }
}

void Start::search(Unit* unit, Zone* zone) {
    unit->theTime += 5;
    for (int i = 1; i <= 9; i++) {
        Coord c = unit->pos + DIRS[i];
        if (c.inBounds(zone->getWidth(), zone->getHeight())) {
            Location* loc = zone->getLocationAt(c);
            if (loc->structure == S_HIDDENDOOR) {
                int c = 10;
                if (unit == player->getUnit()) c += player->getSkillLevel(SKL_SEARC);
                if (rand() % 100 < c) {
                    debankExp(unit, SKL_SEARC, 5);
                    if (unit == player->getUnit()) addMessage("You found a secret door!", BLACK);
                    loc->structure = S_FOUNDDOOR;
                    return;
                }
            }
        }
    }
    if (unit == player->getUnit()) addMessage("You search for a bit.", BLACK);
}

void Start::eatFood(Unit* unit, ItemType* food) {
    int hung = unit->modifyStat(S_HUNGER, food->getStatValue(S_FEED));
    unit->theTime += T_EAT;
    if (unit == player->getUnit()) {
        if (hung > MAX_HUNGER) {
            addStatus("bloated", BROWN, ST_HUNG);
        }
    }
}

void Start::changeLoc(Unit* unit, Zone* zone, Coord loc) {
    zone->getLocationAt(unit->pos)->removeUnit();
    unit->pos = loc;
    zone->getLocationAt(loc)->unit = unit;
    if (player->getUnit() == unit) {
        //if the player changed locations, recalculate the path of chasing mobs
        std::set<std::pair<Unit*, Zone*> >::iterator iter = areaUnits.begin();
        for (; iter != areaUnits.end(); ++iter) {
            Unit* u = iter->first;
            if (iter->second == player->getZone() && u != player->getUnit()) {
                bool newPath = visibilities[u->pos.index(zone->getWidth())];
                if (u->getStatValue(S_AI) != 1) {
                    newPath = false;
                } else if (u->pointOnPath > -1) {
                    Path* aPath = u->currentPath;
                    for (int i = u->pointOnPath; i < aPath->len; i++) {
                        if (unit->pos == aPath->pathLocs[i]) {
                            newPath = false;
                            break;
                        }
                    }
                }
                if (newPath) {
                    makePath(u, unit->pos, iter->second, P_NORMAL);
                    if (u->pointOnPath == -1) {
                        makePath(u, unit->pos, iter->second, P_PASSUNITS);
                    }
                }
            }
        }
        playerFieldOfView(false);
    }
}

void Start::changeLocZ(Unit* unit, Zone* prevZone, Zone* newZone, Coord loc) {
    if (!newZone->isFilled()) {
        Area* area = player->getArea();
        std::pair<int, int> stackTag = newZone->dungeonTag();
        DungeonStack* dungeonStack = area->getDungeonStack(stackTag.first);
        std::vector<std::pair<Unit*, Zone*> >* units = new std::vector<std::pair<Unit*, Zone*> >;
        dungeonStack->genLevel(stackTag.second + 1, units);
        for (unsigned int i = 0; i < units->size(); i++) {
            (*units)[i].first->theTime = world->theTime;
            areaUnits.insert((*units)[i]);
        }
        if (stackTag.second + 1 < dungeonStack->getDepth()) {
            int numStairs = dungeonStack->getNumStairs(stackTag.second);
            for (int i = 0; i < numStairs; i++) {
                Coord coords = dungeonStack->getStairLoc(stackTag.second, i);
                area->addConnection(Connection{coords, coords, newZone, dungeonStack->getZone(stackTag.second + 1)});
            }
        }
        delete units;
    }

    Location* newLoc = newZone->getLocationAt(loc);
    if (newLoc->hasUnit()) {
        std::vector<Coord> possibleLocs;
        for (int i = 1; i <= 9; i++) {
            Coord n = loc + DIRS[i];
            if (n.inBounds(newZone->getWidth(), newZone->getHeight())) {
                Location* nextLoc = newZone->getLocationAt(n);
                if (!nextLoc->hasUnit() && fabs(nextLoc->height - newLoc->height) <= 2 && nextLoc->isOpen()) {
                    possibleLocs.push_back(n);
                }
            }
        }
        if (possibleLocs.empty()) {
            addMessage("There is too much stuff in the way.", BLACK);
            unit->theTime++;
            return;
        }
        int i = rand() % possibleLocs.size();
        loc = possibleLocs[i];
        newLoc = newZone->getLocationAt(loc);
    }
    prevZone->getLocationAt(unit->pos)->removeUnit();
    unit->pos = loc;
    newLoc->unit = unit;
    areaUnits.erase(std::pair<Unit*, Zone*>(unit, prevZone));
    areaUnits.insert(std::pair<Unit*, Zone*>(unit, newZone));
    if (player->getUnit() == unit) {
        player->setZone(newZone);
        //remove all paths from mobs
        playerFieldOfView(true);
    }
}
