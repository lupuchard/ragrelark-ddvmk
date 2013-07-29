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
    int ai = unit->getStatValue(Stat::AI); //the type of AI the monster uses, currently only two.
    bool inZone = zone == player->getZone(); //whether the player is in this zone or not. monsters outside the zone will always just sit still
    Unit* target = player->getUnit();
    switch(ai) {
        case AI_STILL: unit->theTime += 10; break; //just stands still
        case AI_HOSTILE: {
            if (inZone && unit->pointOnPath == -1) { //this means that a path does not exist so one needs to be created first
                if (visibilities[unit->pos.index(zone->getWidth())]) {
                    makePath(unit, target->pos, zone, PATH_NORMAL);
                    if (unit->pointOnPath == -1) {
                        makePath(unit, target->pos, zone, PATH_PASSUNITS);
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
    if (thePath->pathLocs[unit->pointOnPath].x == -PATH_STAIRS) {
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
            //std::cout << "mob move mistake " << dir << " " << cx << " " << cy << " " << unit->x << " " << unit->y << std::endl;
        }
    }
}

void Start::playerWalkStaminaDrain(int* movSpeed, int tim, Unit* unit) {
    if (unit == player->getUnit()) {
        switch(loadStatus) {
            case 0: unit->modifyStat(Stat::STAMINA, -tim / 2); break;
            case 1:
                *movSpeed -= 1;
                unit->modifyStat(Stat::STAMINA, -tim);
                break;
            case 2:
                *movSpeed -= 2;
                unit->modifyStat(Stat::STAMINA, -tim * 3 / 2);
                break;
            case 3:
                *movSpeed -= 3;
                unit->modifyStat(Stat::STAMINA, -tim * 2);
                break;
            default:
                *movSpeed -= 4;
                unit->modifyStat(Stat::STAMINA, -tim * 5 / 2);
                break;
        }
        int stam = unit->getStatValue(Stat::STAMINA);
        if (stam < 2000) {
            *movSpeed -= 2;
        } else if (stam < 5000) {
            *movSpeed -= 1;
        }
    }
}

/* Moves a unit in a zone in a direction */
void Start::moveUnit(Unit* unit, Zone* zone, int dir) {
    if (unit->getStatValue(Stat::CONFUSION)) {
        int newDir = rand() % 11;
        if (newDir == 10) {
            unit->setStat(Stat::CONFUSION, 0);
            if (unit == player->getUnit()) {
                addMessage("You snap out of confusion.", BLACK);
                removeStatus(ST_CONF);
            } else addMessage("The " + unit->name + " is no longer confused.", BLACK);
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
                int swarm = unit->getStatValue(Stat::SWARM);
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
                        makePath(unit, player->getUnit()->pos, zone, PATH_NORMAL);
                    }
                } else if (unit == player->getUnit() || nextLoc->unit == player->getUnit()) {
                    strikeUnit(unit, zone, dir, true);
                } else {
                    unit->theTime += 2;
                    makePath(unit, player->getUnit()->pos, zone, PATH_NORMAL);
                }
                if (unit->pointOnPath > 0) {
                    unit->pointOnPath--;
                }
            } else {
                Location* prevLoc = zone->getLocationAt(unit->pos);
                int prevHeight = prevLoc->getTotalHeight();
                int height = nextLoc->getTotalHeight();
                if (nextLoc->isOpen() && !Tile::get(nextLoc->tile)->blocksMove()) {
                    int movSpeed = unit->getStatValue(Stat::MOVESPEED);
                    normalMove = true;
                    if (prevHeight != height && !unit->getStatValue(Stat::FLY)) {
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
                        tim = actionTimePassed(tim, movSpeed);
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
                            int ai = u->getStatValue(Stat::AI);
                            switch(ai) {
                            case 1: if (visibilities[u->pos.index(zone->getWidth())]) {
                                makePath(u, newPos, zone, PATH_NORMAL);
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
                    int movSpeed = unit->getStatValue(Stat::MOVESPEED);
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
                    int ai = u->getStatValue(Stat::AI);
                    switch(ai) {
                    case 1: if (visibilities[u->pos.index(zone->getWidth())]) {
                        makePath(u, unit->pos, zone, PATH_STAIRS);
                        } break;
                    default: break;
                    }
                }
            }
            addMessage("You go the stairs.", BLACK);
        } else {
            addMessage("A " + unit->name + " comes from the stairs!", OLIVE);
        }
        changeLocZ(unit, zone, sl.z, sl.loc);
    }
}

void Start::hitCMod(Unit* defender, float& damage, float accuracy, int& hitType, BattleSummary& sum) {
    float howLuckyAreYou = (float)rand() / RAND_MAX;
    int fiz = defender->getStatValue(Stat::EVA) - accuracy;
    if (howLuckyAreYou > chanceHit(fiz + 24)) {
        damage *= 5;
        hitType = 4;
        sum.criticality = 4;
    } else if (howLuckyAreYou > chanceHit(fiz + 10)) {
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

String Start::defenderNoun(Unit* attacker, Unit* defender) {
    if (defender == player->getUnit()) {
        if (defender == attacker) return "yourself";
        else return "you";
    } else {
        if (defender == attacker) return "itself";
        else return "the " + defender->name;
    }
}

Color critColors[] = {Color(95, 31, 31), Color(63, 0, 0), MAROON, BRICK, RED};

void Start::pfaf(int stat, int potency, Unit* u) {
    float chance = 1.f / (1 + (u->getStatValue(Stat::RESPOIS) - potency / 5.f) / 10.f);
    if (((float)rand() / RAND_MAX) < chance && u->getStatValue(stat) < potency) {
        u->setStat(stat, potency);
    }
    if (u == player->getUnit()) {
        addStatus("poisoned", PURPLE, ST_POIS);
    }
}

BattleSummary Start::attackUnit(float power, float accuracy, const WeapType* weapType, Unit* defender, Zone* zone, int dir, Flavor flavor) {
    BattleSummary sum = {false, false, false, false};

    float damage = defDam(power, defender->getStatValue(Stat::DEFENSE));
    damage *= ((float)rand() / RAND_MAX) / 8 + .9375;
    int hitType;
    hitCMod(defender, damage, accuracy, hitType, sum);
    if (!weapType) weapType = ItemType::getWeapType(0);
    raga.rAttack(defender->pos, dir, weapType->damageType, hitType);

    if (damage) {
        sum.hit = true;

        for (std::map<FlavorClass, int>::iterator iter = flavor.flavors.begin(); iter != flavor.flavors.end(); ++iter) {
            switch(iter->first) {
            case F_POISON_PHYS: pfaf(Stat::POIS_PHYS, iter->second, defender); break;
            case F_POISON_MENT: pfaf(Stat::POIS_MENT, iter->second, defender); break;
            case F_POISON_REGEN: pfaf(Stat::POIS_REGEN, iter->second, defender); break;
            case F_POISON_EXTRA: pfaf(Stat::POIS_EXTRA, iter->second, defender); break;
            case F_CONFUSION: {
                float chance = 1.f / (1 + (/* TODO concentration defender->getStatValue(Stat::RESPOIS)*/5 - iter->second) / 10.f);
                if (((float)rand() / RAND_MAX) < chance) {
                    defender->setStat(Stat::CONFUSION, 1);
                }
                if (defender == player->getUnit()) addStatus("confused", BROWN, ST_CONF);
            } break;
            default: break;
            }
        }

        int hp = defender->modifyStat(Stat::HP, -(int)damage);
        int splatterChance;
        if (hp <= 0) {
            splatterChance = 10;
        } else {
            splatterChance = 10 * damage / hp;
        }
        splatterChance *= defender->getStatValue(Stat::SPLATTER);
        if (rand() % 200 < splatterChance) {
            makeSplatter(defender, zone, defender->pos);
        }

        if (hp <= 0) {
            if (defender != player->getUnit()) player->bankXp(defender->getStatValue(Stat::EXP));
            killUnit(defender, zone);
            sum.killed = true;
        } else {
            reactToAttack(defender, dir, zone);
        }
    }
    return sum;
}

void Start::strikeUnit(Unit* unit, Zone* zone, int dir, bool safe) {
    if (unit == player->getUnit() && unit->getStatValue(Stat::STAMINA) < 2000) {
        addMessage("You are too exausted to attack!", GRAY);
    } else {
        Coord newPos = unit->pos + DIRS[dir];
        if (safe || (newPos.inBounds(zone->getWidth(), zone->getHeight()))) {
            Location* loc = zone->getLocationAt(newPos);
            if (loc->hasUnit()) {
                unit->setEnemy(loc->unit);
                Unit* defender = loc->unit;

                WeapType* weapType = NULL;
                Skill* skill = NULL;
                bool unarmed = true;
                if (unit == player->getUnit()) {
                    for (int i = 0; i < primeFolder->getEquips()->getNumItems(); i++) {
                        ItemTypeType* itemTypeType = primeFolder->getEquips()->getItem(i)->getType()->getType();
                        if (itemTypeType->weapType && !itemTypeType->ranged) {
                            weapType = itemTypeType->weapType;
                            skill = itemTypeType->skill;
                            unarmed = false;
                            break;
                        }
                    }
                } else if (unit->equipment) {
                    for (int i = 0; i < unit->equipment->len; i++) {
                        ItemTypeType* itemTypeType = unit->equipment->equips[i].getType()->getType();
                        if (itemTypeType->weapType && !itemTypeType->ranged) {
                            weapType = itemTypeType->weapType;
                            unarmed = false;
                            break;
                        }
                    }
                }


                unit->theTime += actionTimePassed(T_ATTACK, unit->getStatValue(Stat::ATTACKSPEED));
                BattleSummary bs;
                if (unit == player->getUnit()) {
                    unit->modifyStat(Stat::STAMINA, -T_ATTACK);
                }

                String u1name;
                String u2name = defenderNoun(unit, defender);

                Flavor f;
                FlavorClass fc = (FlavorClass)unit->getStatValue(Stat::AFFLICTION);
                if (fc) f.add(fc, unit->getStatValue(Stat::AFFLICTION_POTENCY));

                if (unarmed) {
                    weapType = ItemType::getWeapType(unit->getStatValue(Stat::UNARMED));
                    bs = attackUnit(unit->getStatValue(Stat::UNARMDAMAGE), unit->getStatValue(Stat::ACC), weapType, defender, zone, dir, f);
                } else {
                    float dam = unit->getStatValue(Stat::MELDAMAGE);
                    float acc = unit->getStatValue(Stat::ACC);
                    if (skill) {
                        dam += player->getSkillLevel(skill) * .1;
                        acc += player->getSkillLevel(skill) * .1;
                        sapExp(unit, defender, skill, 2);
                    }
                    bs = attackUnit(dam, acc, weapType, defender, zone, dir, f);
                }

                Color c = critColors[bs.criticality];
                String verb;
                String extra;
                if (unit == player->getUnit()) {
                    u1name = "you";
                    verb = weapType->getVerb(bs.criticality, false);
                } else {
                    u1name = "the " + unit->name;
                    c.green += c.red / 2;
                    verb = weapType->getVerb(bs.criticality, true);
                }

                if (!bs.killed && bs.hit) {
                    if (bs.dodge) sapExp(defender, unit, Stat::getSkill(SKL_DODGE), 2);
                    if (unarmed) sapExp(unit, defender, skll(SKL_UNARMED), 3);
                } else if (bs.killed) {
                    extra += " to death";
                }
                addMessage(capitalize(u1name + " " + verb + " " + u2name + extra + "."), c);
            }
        }
    }
}

void Start::shootUnit(Unit* attacker, Item shooter, Item beingShot, Unit* defender, Zone* zone) {
    if (attacker == player->getUnit() && attacker->getStatValue(Stat::STAMINA) < 1500) {
        addMessage("You are too exausted to shoot!", GRAY);
    } else {
        attacker->setEnemy(defender);
        attacker->theTime += actionTimePassed(T_ATTACK, attacker->getStatValue(Stat::ATTACKSPEED));
        if (attacker == player->getUnit()) {
            attacker->modifyStat(Stat::STAMINA, -(T_ATTACK / 2));
        }
        String verb, u1name;
        String u2name = defenderNoun(attacker, defender);
        //WeapType* weapType = shooter.getType()->getType()->weapType;
        BattleSummary bs = projectItem(beingShot, attacker->getStatValue(Stat::RANDAMAGE), attacker->getStatValue(Stat::ACC), zone, defender->pos, attacker->pos);
        //BattleSummary bs = attackUnit(attacker->getStatValue(Stat::RANDAMAGE), attacker->getStatValue(Stat::ACC), weapType, defender, zone, 0);
        if (!bs.killed) {
            if (bs.dodge) sapExp(defender, attacker, Stat::getSkill(SKL_DODGE), 2);
            if (bs.hit) sapExp(attacker, defender, Stat::getSkill(SKL_SHOOT), 2);
        }

        WeapType* weapType = beingShot.getType()->getType()->weapType;
        Color c = critColors[bs.criticality];

        if (attacker == player->getUnit()) {
            u1name = "you";
            verb = weapType->getVerb(bs.criticality, false);
        } else {
            u1name = "the " + attacker->name + " ";
            c.green += c.red / 2;
            verb = weapType->getVerb(bs.criticality, true);
        }
        addMessage(capitalize(u1name + " " + verb + " " + u2name + "."), c);
    }
}

void Start::throwItem(Item item, Unit* unit, Coord target, Zone* zone) {
    String u2name;
    Location* loc = zone->getLocationAt(target);
    if (loc->hasUnit()) u2name = defenderNoun(unit, loc->unit);

    unit->theTime += actionTimePassed(T_ATTACK, unit->getStatValue(Stat::ATTACKSPEED));
    BattleSummary bs = projectItem(item, 0, unit->getStatValue(Stat::ACC) + item.getType()->getStatValue(Stat::THRO) - 2, zone, target, unit->pos);
    if (!bs.killed) {
        if (bs.dodge) sapExp(zone->getLocationAt(target)->unit, unit, Stat::getSkill(SKL_DODGE), 2);
        if (bs.hit) sapExp(unit, zone->getLocationAt(target)->unit, Stat::getSkill(SKL_THROW), 2);
    }
    if (bs.hit || bs.dodge) {
        String verb, u1name;
        Color c = critColors[bs.criticality];

        if (unit == player->getUnit()) {
            u1name = "you throw the ";
            verb = ItemType::getWeapType(0)->getVerb(bs.criticality, false);
        } else {
            u1name = "the " + unit->name + " throws tho ";
            c.green += c.red / 2;
            verb = ItemType::getWeapType(0)->getVerb(bs.criticality, true);
        }
        addMessage(capitalize(u1name + item.getType()->getName() + " at " + u2name + " and " + verb + "."), c);
    }
}

BattleSummary Start::projectItem(Item item, int power, int accuracy, Zone* zone, Coord to, Coord from) {
    ItemType* itemType = item.getType();
    Location* loc = zone->getLocationAt(to);
    if (loc->unit) {
        BattleSummary bs = attackUnit(power + itemType->getStatValue(Stat::DAM), accuracy, itemType->getType()->weapType, loc->unit, zone, 0); //TODO dir
        if (bs.hit) {
            if ((float)rand() / RAND_MAX > itemType->getStatValue(Stat::BREAK) / 1000.f) {
                addItemToPlace(to, zone, item);
            }
        } else addItemToPlace(to, zone, item);
        return bs;
    } else {
        addItemToPlace(to, zone, item);
        return BattleSummary{0, 0, 0, 0};
    }
}

void Start::reactToAttack(Unit* unit, int dir, Zone* zone) {
    int ai = unit->getStatValue(Stat::AI);
    switch(ai) {
        case AI_PASSIVE: {
            makePath(unit, unit->pos - DIRS[dir], zone, PATH_FLEE);
            unit->theTime = world->theTime - T_WALK_ST;
            } break;
        default: break;
    }
}

void Start::makeSplatter(Unit* unit, Zone* zone, Coord pos) {
    Location* loc = zone->getLocationAt(pos);
    int bloodColor = unit->getStatValue(Stat::BLOOD);
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
    if (unit == player->getUnit()) {
        addMessage("GAME OVER", BLACK);
        addStatus("dead", WHITE, ST_DEAD);
        state = STATE_DEAD;
        return;
    }
    Location* locOfDeath = zone->getLocationAt(unit->pos);
    locOfDeath->removeUnit();
    areaUnits.erase(std::pair<Unit*, Zone*>(unit, zone));
    if (unit->equipment) {
        for (int i = 0; i < unit->equipment->len; i++) {
            locOfDeath->addItem(unit->equipment->equips[i]);
        }
    }
    int split = unit->getStatValue(Stat::SPLIT);
    if (split) {
        for (int i = 0; i < split; i++) {
            Unit* newUnit = mobSpawner->spawnMob(mobSpawner->getMob(unit->getStatValue(Stat::SPAWN)), zone, unit->pos, world->theTime);
            if (newUnit) {
                areaUnits.insert(std::pair<Unit*, Zone*>(newUnit, zone));
                newUnit->theTime = world->theTime;
                raga.rMoveLoc(newUnit, unit->pos, newUnit->pos);
            } else break;
        }
    }
    unit->pos = Coord(-2, -2);
    if (unit->graphic.border) unitDeleteList.push_back(unit);
    else delete unit;
}

void Start::openDoor(Unit* unit, Zone* zone, int dir, bool safe) {
    Coord doorPos = unit->pos + DIRS[dir];
    if (unit->getStatValue(Stat::AI) > 1 && (safe || doorPos.inBounds(zone->getWidth(), zone->getHeight()))) {
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
                //if (unit == player->getUnit()) c += player->getSkillLevel(SKL_SEARC);
                if (rand() % 100 < c) {
                    //debankExp(unit, SKL_SEARC, 5);
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
    int hung = unit->modifyStat(Stat::HUNGER, food->getStatValue(Stat::FEED));
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
                if (u->getStatValue(Stat::AI) != 1) {
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
                    makePath(u, unit->pos, iter->second, PATH_NORMAL);
                    if (u->pointOnPath == -1) {
                        makePath(u, unit->pos, iter->second, PATH_PASSUNITS);
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
