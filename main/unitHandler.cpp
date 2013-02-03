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

int actionTimePassed(int time, int speed) {
    return max((int)(time * pow(.95, speed - 8)), 1);
}

//ok I WILL comment this method OKAY then eventuallie all the methods will be comment and success
//this method runs to tell the unit in the zone what to do, it chooses it's action
void Start::ai(Unit* unit, Zone* zone) {
    int ai = unit->getStatValue(S_AI); //the type of AI the monster uses, currently only two.
    bool inZone = zone == player->getZone(); //whether the player is in this zone or not. monsters outside the zone will always just sit still
    Unit* target = player->getUnit();
    switch(ai) {
        case AI_STILL: unit->theTime += 10; break;//just stands still
        case AI_HOSTILE: {
            if (inZone && unit->pointOnPath == -1) { //this means that a path does not exist so one needs to be created first
                if (visibilities[unit->x + zone->getWidth() * unit->y]) {
                    makePath(unit, target->x, target->y, zone, P_NORMAL);
                    if (unit->pointOnPath == -1) {
                        makePath(unit, target->x, target->y, zone, P_PASSUNITS);
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
                        int tx = unit->x + xDirs[foon];
                        int ty = unit->y + yDirs[foon];
                        if (tx >= 0 && ty >= 0 && tx < zone->getWidth() && ty < zone->getHeight() && !zone->getLocationAt(tx, ty)->hasUnit()) {
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
    path* thePath = unit->currentPath;
    unit->pointOnPath++;
    if (thePath->pathXs[unit->pointOnPath] == -P_STAIRS) {
        goTheStairs(unit, zone);
    } else {
        int dir = 1;
        int cx = thePath->pathXs[unit->pointOnPath] - unit->x;
        int cy = thePath->pathYs[unit->pointOnPath] - unit->y;
        for (; dir <= 9; dir++) { //this loop is used to determine the direction so that moveUnit needs one
            if (xDirs[dir] == cx && yDirs[dir] == cy) {
                break;
            }
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
                player->gainSkillExp(SKL_LIFT, player->takeFromXpBank(1));
                break;
            case 2:
                *movSpeed -= 2;
                unit->modifyStat(S_STAMINA, -tim * 3 / 2);
                player->gainSkillExp(SKL_LIFT, player->takeFromXpBank(2));
                break;
            case 3:
                *movSpeed -= 3;
                unit->modifyStat(S_STAMINA, -tim * 2);
                player->gainSkillExp(SKL_LIFT, player->takeFromXpBank(3));
                break;
            default:
                *movSpeed -= 4;
                unit->modifyStat(S_STAMINA, -tim * 5 / 2);
                player->gainSkillExp(SKL_LIFT, player->takeFromXpBank(4));
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
                addMessage("You snap out of confusion.", black);
                removeStatus(ST_CONF);
            }
        } else if (newDir) dir = newDir;
    }
    if (!(unit == player->getUnit() && loadStatus == 4)) {
        short x = unit->x;
        short y = unit->y;
        short newX = x + xDirs[dir];
        short newY = y + yDirs[dir];
        bool normalMove = false;
        bool diags = dir % 2;
        if (newX >= 0 && newY >= 0 && newX < zone->getWidth() && newY < zone->getHeight()) {
            Location* nextLoc = zone->getLocationAt(newX, newY);
            if (nextLoc->hasUnit()) {
                if (unit == player->getUnit() || nextLoc->unit == player->getUnit()) {
                    strikeUnit(unit, zone, dir, true);
                } else {
                    unit->theTime += 2;
                    makePath(unit, player->getUnit()->x, player->getUnit()->y, zone, P_NORMAL);
                }
                if (unit->pointOnPath > 0) {
                    unit->pointOnPath--;
                }
            } else {
                Location* prevLoc = zone->getLocationAt(x, y);
                int prevHeight = prevLoc->getTotalHeight();
                int height = nextLoc->getTotalHeight();
                if (nextLoc->isOpen() && !zone->getTileAt(newX, newY)->blocksMove()) {
                    int movSpeed = unit->getStatValue(S_MOVESPEED);
                    normalMove = true;
                    if (prevHeight != height) {
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
                        changeLoc(unit, zone, newX, newY);
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
            speLoc sl = player->getArea()->moveZones(zone, newX, newY);
            if (sl.x != -1) {
                if (unit == player->getUnit()) {
                    for (set<pair<Unit*, Zone*> >::iterator i = areaUnits.begin(); i != areaUnits.end(); i++) {
                        Zone* z = i->second;
                        if (z == zone) {
                            Unit* u = i->first;
                            int ai = u->getStatValue(S_AI);
                            switch(ai) {
                            case 1: if (visibilities[u->x + zone->getWidth() * u->y]) {
                                cout << "happen " << newX << " " << newY << endl;
                                makePath(u, newX, newY, zone, P_NORMAL);
                                } break;
                            default: break;
                            }
                        }
                    }
                }
                changeLocZ(unit, zone, sl.z, sl.x, sl.y);
            }
        }
        if (normalMove && zone == player->getZone()) {
            raga.rMoveDir(unit, dir, x, y);
        }
    }
}

void Start::pushRock(Unit* unit, Zone* zone, int dir) {
    short pathX = unit->x + xDirs[dir] * 2;
    short pathY = unit->y + yDirs[dir] * 2;
    bool diags = dir % 2;
    if (pathX >= 0 && pathY >= 0 && pathX < zone->getWidth() && pathY < zone->getHeight()) {
        Location* rockLoc = zone->getLocationAt(unit->x + xDirs[dir], unit->y + yDirs[dir]);
        if (rockLoc->structure == S_ROCK && fabs(rockLoc->height - zone->getLocationAt(unit->x, unit->y)->height) <= 2) {
            Location* pathLoc = zone->getLocationAt(pathX, pathY);
            int diff = pathLoc->height - rockLoc->height;
            if (diff == 2) {
                addMessage("It is too difficult to push the rock up this hill.", gray);
            } else if (diff <= 1) {
                if (pathLoc->hasUnit() || pathLoc->structure != S_NONE) {
                    addMessage("There is something in the way of the rock's path.", gray);
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
                    raga.rMoveDir(unit, dir, unit->x, unit->y);
                    changeLoc(unit, zone, unit->x + xDirs[dir], unit->y + yDirs[dir]);
                }
            }
        }
    }
}

void Start::goTheStairs(Unit* unit, Zone* zone) {
    speLoc sl = player->getArea()->moveConnection(zone, unit->x, unit->y);
    if (sl.x != -1) {
        if (unit == player->getUnit()) {
            for (set<pair<Unit*, Zone*> >::iterator i = areaUnits.begin(); i != areaUnits.end(); i++) {
                Zone* z = i->second;
                if (z == zone) {
                    Unit* u = i->first;
                    int ai = u->getStatValue(S_AI);
                    switch(ai) {
                    case 1: if (visibilities[u->x + zone->getWidth() * u->y]) {
                        makePath(u, unit->x, unit->y, zone, P_STAIRS);

                        } break;
                    default: break;
                    }
                }
            }
        } else {
            addMessage("A " + unit->name + " comes from the stairs!", olive);
        }
        changeLocZ(unit, zone, sl.z, sl.x, sl.y);
    }
}

void Start::hitCMod(Unit* unit, float& damage, color& c, int& hitType, string& verb, int unarmedAttackType) {
    float howLuckyAreYou = (float)rand() / RAND_MAX;
    if (howLuckyAreYou > unit->getStatValueF(S_MCRITC)) {
        damage *= 5;
        hitType = 4; c = red;
        switch(unarmedAttackType) {
            case WEAP_CLAWS: verb = "rend"; break;
            case WEAP_SPEAR: verb = "impale"; break;
            default: verb = "megacrit"; break;
        }
        sapExp(unit, unit->getEnemy(), SKL_CRIT, 3);
    } else if (howLuckyAreYou > unit->getStatValueF(S_CRITC)) {
        damage *= 2;
        hitType = 3; c = brick;
        switch(unarmedAttackType) {
            case WEAP_FISTS: verb = "kick"; break;
            case WEAP_TENTACLE: verb = "tentacle slap"; break;
            case WEAP_SLAM: verb = "slam"; break;
            case WEAP_BITE: verb = "chomp"; break;
            case WEAP_CLUB: verb = "clobber"; break;
            case WEAP_DAGGER: verb = "stab"; break;
            case WEAP_AXE: verb = "chop"; break;
            default: verb = "crit"; break;
        }
        sapExp(unit, unit->getEnemy(), SKL_CRIT, 1);
    } else if (howLuckyAreYou > unit->getStatValueF(S_HITC)) {
        hitType = 2; c = maroon;
        switch(unarmedAttackType) {
            case WEAP_CLAWS: verb = "claw"; break;
            case WEAP_FISTS: verb = "punch"; break;
            case WEAP_HEAD: verb = "headbutt"; break;
            case WEAP_BITE: verb = "bite"; break;
            case WEAP_CLUB: verb = "smack"; break;
            case WEAP_AXE: verb = "axe"; break;
            case WEAP_SPEAR: verb = "jab"; break;
            case WEAP_SCYTHE: verb = "slash"; break;
            default: verb = "hit"; break;
        }
    } else if (howLuckyAreYou > unit->getStatValueF(S_SCRAPEC)) {
        damage /= 2;
        hitType = 1; c = dark(maroon);
        switch(unarmedAttackType) {
            case WEAP_CLAWS: verb = "scratch"; break;
            case WEAP_TENTACLE: verb = "rub"; break;
            case WEAP_FISTS: verb = "pap"; break;
            case WEAP_CLUB: verb = "tap"; break;
            case WEAP_SPEAR: verb = "poke"; break;
            default: verb = "scrape"; break;
        }
        sapExp(unit, unit->getEnemy(), SKL_CRIT, 1);
        sapExp(unit->getEnemy(), unit, SKL_DODGE, 1);
    } else {
        damage = 0;
        verb = "miss";
        hitType = 0; c = dark(salmon);
        sapExp(unit->getEnemy(), unit, SKL_DODGE, 1);
    }
}

void Start::strikeUnit(Unit* unit, Zone* zone, int dir, bool safe) {
    if (unit == player->getUnit() && unit->getStatValue(S_STAMINA) < 2000) {
        addMessage("You are too exausted to attack!", gray);
    } else {
        short x = unit->x;
        short y = unit->y;
        short newX = x + xDirs[dir];
        short newY = y + yDirs[dir];
        if (safe || (newX >= 0 && newY >= 0 && newX < zone->getWidth() && newY < zone->getHeight())) {
            Location* loc = zone->getLocationAt(newX, newY);
            if (loc->hasUnit()) {
                attackUnit(unit, loc->unit, zone, dir, ATT_STRIKE);
            }
        }
    }
}

const int damStats[] = {S_MELDAMAGE, S_RANDAMAGE, S_SPEDAMAGE};
const SkillType damSkills[] = {SKL_MELEE, SKL_RANGE, SKL_CONC};
void Start::attackUnit(Unit* attacker, Unit* defender, Zone* zone, int dir, AttackType attackType) {
    attacker->setEnemy(defender);

    string verb;
    string u1name;
    string u2name;


    bool unarmed = false;
    int weapType = 0;
    if (attackType == ATT_STRIKE) {
        unarmed = true;
        if (attacker == player->getUnit()) {
            Item* item = primeFolder->getEquips()->getItem(E_RHAND);
            ItemType* itemType = getItemType(item->itemType);
            if (itemType->getType() != I_SLOT && item) {
                if (!typeRanges[item->itemType]) {
                    weapType = itemType->getStatValue(S_DTYPE);
                    unarmed = false;
                }
            }
        } else if (attacker->equipment) {
            for (int i = 0; i < attacker->equipment->len; i++) {
                Item item = attacker->equipment->equips[i];
                ItemType* itemType = getItemType(item.itemType);
                int slot = typeSlots[itemType->getType()];
                if (slot == E_RHAND || slot == E_BHANDS || slot == E_BBHANDS) {
                    weapType = itemType->getStatValue(S_DTYPE);
                    unarmed = false;
                    break;
                }
            }
        }
        if (unarmed) {
            weapType = attacker->getStatValue(S_UNARMED);
        }
    } else if (attackType == ATT_SHOOT) weapType = DAMT_PIERCE;
    else if (attackType == ATT_SPELL) weapType = DAMT_SPELL;

    float damage;
    if (unarmed) damage = attacker->getStatValueF(S_UNARMDAMAGE);
    else damage = attacker->getStatValueF(damStats[attackType]);

    damage *= ((float)rand() / RAND_MAX) / 8 + .9375;
    int hitType;
    color c;

    hitCMod(attacker, damage, c, hitType, verb, weapType);

    //fooey

    if (attacker == player->getUnit()) {
        if (attackType == ATT_STRIKE) u1name = "you";
        else if (attackType == ATT_SHOOT) u1name = "you fire and";
    } else {
        if (attackType == ATT_STRIKE) u1name = "the " + attacker->name;
        else if (attackType == ATT_SHOOT) u1name = "the " + attacker->name + " shoots and";
        c.green += c.red / 2;
        verb = pluralize(verb);
    }
    if (defender == player->getUnit()) {
        if (defender == attacker) u2name = "yourself";
        else u2name = "you";
    } else {
        if (defender == attacker) u2name = "itself";
        else u2name = "the " + defender->name;
    }

    string extra;
    raga.rAttack(defender->x, defender->y, dir, weapDamTypes[weapType], hitType);

    attacker->theTime += actionTimePassed(T_ATTACK, attacker->getStatValue(S_ATTACKSPEED));
    if (attacker == player->getUnit()) {
        if (attackType == ATT_STRIKE) attacker->modifyStat(S_STAMINA, -T_ATTACK);
        else attacker->modifyStat(S_STAMINA, -(T_ATTACK / 2));
    }

    if (damage) {
        int hp = defender->modifyStat(S_HP, -(int)damage);
            if (hp > 0) {
                int conditionAffectC = attacker->getStatValue(S_ATTCONDCHANCE);
                if (conditionAffectC > 0 && rand() % 100 <= conditionAffectC) {
                    int conditionAffect = attacker->getStatValue(S_ATTCOND);
                    if (defender == player->getUnit()) {
                        switch(conditionAffect) {
                        case C_CONFUSION: extra = " and confuses";
                        addStatus("Confused", maroon, ST_CONF);
                        break;
                        case C_POISON: extra = " and stings"; break;
                        default: break;
                    }
                }
                if (conditionAffect >= C_POISON && conditionAffect < C_POISON + 16) {
                    applyPoison(conditionAffect, 10, defender);
                } else {
                    defender->setCondition(conditionAffect, true);
                }
            }
        }
        if (unarmed) sapExp(attacker, defender, SKL_UNARM, 1);
        else sapExp(attacker, defender, damSkills[attackType], 1);
        sapExp(defender, attacker, SKL_FORT, 1);
        int splatterChance;
        if (hp <= 0) {
            splatterChance = 10;
        } else {
            splatterChance = 10 * damage / hp;
            if (attackType == ATT_SHOOT) splatterChance /= 2;
        }
        splatterChance *= defender->getStatValue(S_SPLATTER);
        if (rand() % 200 < splatterChance) {
            makeSplatter(defender, defender->x, defender->y);
        }

        if (hp <= 0) {
            if (defender != player->getUnit()) player->bankXp(defender->getStatValue(S_EXP));
            killUnit(defender, zone);
        } else {
            reactToAttack(defender, attacker, zone);
        }
    }
    addMessage(capitalize(u1name + " " + verb + " " + u2name + extra + "."), c);
}

void Start::shootUnit(Unit* attacker, Unit* defender, Zone* zone) {
    if (attacker == player->getUnit() && attacker->getStatValue(S_STAMINA) < 1500) {
        addMessage("You are too exausted to shoot!", gray);
    } else {
        attackUnit(attacker, defender, zone, 0, ATT_SHOOT);
    }
}

void Start::reactToAttack(Unit* unit, Unit* attacker, Zone* zone) {
    int ai = unit->getStatValue(S_AI);
    switch(ai) {
        case AI_PASSIVE: {
            makePath(unit, attacker->x, attacker->y, zone, P_FLEE);
            unit->theTime = attacker->theTime - T_WALK_ST;
            } break;
        default: break;
    }
}

int poisonWeights[] = {1, 2, 4, 1, 1, 1, 1, 1, 1, 3, 3, 1, 2, 2, 3, 3};
void Start::applyPoison(int condition, int duration, Unit* unit) {
    int r = rand() % 100;
    int res = unit->getStatValue(S_RESPOIS);
    if (r < res / 5) {
        if (unit == player->getUnit()) addMessage("You resist the poison!", black);
        return;
    } else if (r < res) {
        if (unit == player->getUnit()) addMessage("You slightly resist the poison.", black);
        duration /= 2;
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
            else if (i >= 15 && i >= 18) befuddle = true;
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
        string statusString = "Poison";
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
        addStatus(statusString, dark(purple), ST_POIS);
    }
}

//TODO first make new move animation then finish animations in unit spawning then finish slime/main mob spawning

void Start::makeSplatter(Unit* unit, int x, int y) {
    int i = x + player->getZone()->getWidth() * y;
    if (splatters[i] == 255) {
        splatters[i] = unit->getStatValue(S_BLOOD) * 32 - 1;
    }
    splatters[i] += rand() % 2 + 1;
}

void Start::killUnit(Unit* unit, Zone* zone) {
    Location* locOfDeath = zone->getLocationAt(unit->x, unit->y);
    locOfDeath->removeUnit();
    areaUnits.erase(pair<Unit*, Zone*>(unit, zone));
    if (unit->equipment) {
        for (int i = 0; i < unit->equipment->len; i++) {
            locOfDeath->addItem(unit->equipment->equips[i]);
        }
    }
    int split = unit->getStatValue(S_SPLIT);
    if (split) {
        for (int i = 0; i < split; i++) {
            Unit* newUnit = mobSpawner->spawnMobSpeTag(unit->getStatValue(S_SPAWN), zone, unit->x, unit->y, world->theTime);
            if (newUnit) {
                areaUnits.insert(pair<Unit*, Zone*>(newUnit, zone));
                newUnit->theTime = world->theTime;
                raga.rMoveLoc(newUnit, unit->x, unit->y, newUnit->x, newUnit->y);
            } else break;
        }
    }
    unit->x = -2;
    unit->y = -2;
    if (unit == player->getUnit()) {
        addMessage("GAME OVER HAHA", black);
    }
    if (unit->g.border) unitDeleteList.push_back(unit);
    else delete unit;
}

void Start::openDoor(Unit* unit, Zone* zone, int dir, bool safe) {
    short doorX = unit->x + xDirs[dir];
    short doorY = unit->y + yDirs[dir];
    if (unit->getStatValue(S_AI) > 1 && (safe || (doorX >= 0 && doorY >= 0 && doorX < zone->getWidth() && doorY < zone->getHeight()))) {
        Location* nextLoc = zone->getLocationAt(doorX, doorY);
        int str = nextLoc->structure;
        if (isClosedDoor(str)) {
            nextLoc->structure = str + 1;
            unit->theTime += T_OPENDOOR;
            if (unit == player->getUnit()) {
                addMessage("You open the door.", black);
            } else {
                addMessage("A " + unit->name + " opens a door.", olive);
            }
            if (unit == player->getUnit()) {
                playerFieldOfView(false);
            }
        }
    }
}

void Start::closeDoor(Unit* unit, Zone* zone, int dir, bool safe) {
    short doorX = unit->x + xDirs[dir];
    short doorY = unit->y + yDirs[dir];
    if (safe || (doorX >= 0 && doorY >= 0 && doorX < zone->getWidth() && doorY < zone->getHeight())) {
        Location* nextLoc = zone->getLocationAt(doorX, doorY);
        int str = nextLoc->structure;
        if (isOpenDoor(str)) {
            if (nextLoc->hasUnit()) {
                addMessage("There's something in the way!", gray);
            } else {
                nextLoc->structure = str - 1;
                unit->theTime += T_OPENDOOR;
                if (unit == player->getUnit()) {
                    addMessage("You close the door.", black);
                } else {
                    addMessage("A " + unit->name + " closes a door.", olive);
                }
            }
            if (unit == player->getUnit()) {
                playerFieldOfView(false);
            }
        }
    }
}

void Start::eatFood(Unit* unit, ItemType* food) {
    int hung = unit->modifyStat(S_HUNGER, food->getStatValue(S_FEED));
    unit->theTime += T_EAT;
    if (unit == player->getUnit()) {
        if (hung > MAX_HUNGER) {
            addStatus("bloated", brown, ST_HUNG);
        }
    }
}

void Start::changeLoc(Unit* unit, Zone* zone, int x, int y) {
    zone->getLocationAt(unit->x, unit->y)->removeUnit();
    (*unit).x = x;
    (*unit).y = y;
    zone->getLocationAt(x, y)->unit = unit;

    if (player->getUnit() == unit) {
        //if the player changed locations, recalculate the path of chasing mobs
        set<pair<Unit*, Zone*> >::iterator iter = areaUnits.begin();
        for (; iter != areaUnits.end(); iter++) {
            Unit* u = iter->first;
            if (iter->second == player->getZone() && u != player->getUnit()) {
                bool newPath = visibilities[u->x + u->y * zone->getWidth()];
                if (u->getStatValue(S_AI) != 1) {
                    newPath = false;
                } else if (u->pointOnPath > -1) {
                    path* aPath = u->currentPath;
                    for (int i = u->pointOnPath; i < aPath->len; i++) {
                        int pathX = aPath->pathXs[i];
                        int pathY = aPath->pathYs[i];
                        if (unit->x == pathX && unit->y == pathY) {
                            newPath = false;
                            break;
                        }
                    }
                }
                if (newPath) {
                    makePath(u, unit->x, unit->y, iter->second, P_NORMAL);
                    if (u->pointOnPath == -1) {
                        makePath(u, unit->x, unit->y, iter->second, P_PASSUNITS);
                    }
                }
            }
        }
        playerFieldOfView(false);
    }
}

void Start::changeLocZ(Unit* unit, Zone* prevZone, Zone* newZone, int x, int y) {
    if (!newZone->isFilled()) {

        Area* area = player->getArea();
        pair<int, int> stackTag = newZone->dungeonTag();
        DungeonStack* dungeonStack = area->getDungeonStack(stackTag.first);
        vector<pair<Unit*, Zone*> >* units = new vector<pair<Unit*, Zone*> >;
        dungeonStack->genLevel(stackTag.second + 1, units);
        for (unsigned int i = 0; i < units->size(); i++) {
            (*units)[i].first->theTime = world->theTime;
            areaUnits.insert((*units)[i]);
        }
        if (stackTag.second + 1 < dungeonStack->getDepth()) {
            int numStairs = dungeonStack->getNumStairs(stackTag.second);
            for (int i = 0; i < numStairs; i++) {
                pair<short, short> coords = dungeonStack->getStairLoc(stackTag.second, i);
                area->addConnection(connection{coords.first, coords.second, coords.first, coords.second, newZone, dungeonStack->getZone(stackTag.second + 1)});
            }
        }
        delete units;
    }

    Location* newLoc = newZone->getLocationAt(x, y);
    if (newLoc->hasUnit()) {
        vector<pair<int, int> > possibleLocs;
        for (int i = 1; i <= 9; i++) {
            int nx = x + xDirs[i];
            int ny = y + yDirs[i];
            if (nx >= 0 && ny >= 0 && nx < newZone->getWidth() && ny < newZone->getHeight()) {
                Location* nextLoc = newZone->getLocationAt(nx, ny);
                if (!nextLoc->hasUnit() && fabs(nextLoc->height - newLoc->height) <= 2 && nextLoc->isOpen()) {
                    possibleLocs.push_back(pair<int, int>(nx, ny));
                }
            }
        }
        if (possibleLocs.size() == 0) {
            addMessage("There is too much stuff in the way.", black);
            unit->theTime++;
            return;
        }
        int i = rand() % possibleLocs.size();
        x = possibleLocs[i].first;
        y = possibleLocs[i].second;
        newLoc = newZone->getLocationAt(x, y);
    }
    prevZone->getLocationAt(unit->x, unit->y)->removeUnit();
    (*unit).x = x;
    (*unit).y = y;
    newLoc->unit = unit;
    areaUnits.erase(pair<Unit*, Zone*>(unit, prevZone));
    areaUnits.insert(pair<Unit*, Zone*>(unit, newZone));
    if (player->getUnit() == unit) {
        player->setZone(newZone);
        //remove all paths from mobs
        playerFieldOfView(true);
    }
    for (int i = 0; i < MAX_ZONE_SIZE; i++) {
        splatters[i] = 255;
    }
}
