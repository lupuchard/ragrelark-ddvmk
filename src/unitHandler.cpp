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

const int unarmedDamageTypes[] = {0, 2, 1, 1, 1, 1};

int actionTimePassed(int time, int speed) {
    return max((int)(time * pow(.95, speed - 8)), 1);
}

//ok I WILL comment this method OKAYS then eventuallies all the methods will be comment and success
//this method runs to tell the unit in the zone what to do, it chooses it's action
void Start::ai(Unit* unit, Zone* zone) {
    int ai = unit->getStatValue(S_AI); //the type of AI the monster uses, currently only two.
    //0 = stand still, 1 = mindless hostile, 2 = intelligent hostile
    bool inZone = zone == player->getZone(); //whether the player is in this zone or not. monsters outside the zone will always just sit still
    Unit* target = player->getUnit();
    switch(ai) {
        case 0: unit->theTime += 10; break;//just stands still
        case 1: {
            if (inZone && unit->pointOnPath == -1) { //this means that a path does not exist so one needs to be created first
                if (visibilities[unit->x + zone->getWidth() * unit->y]) {
                    makePath(unit, target->x, target->y, zone, P_NORMAL);
                    if (unit->pointOnPath == -1) {
                        makePath(unit, target->x, target->y, zone, P_PASSUNITS);
                    }
                }
            }
            if (unit->pointOnPath > -1) { //it performs the check again because a path may have failed to be created (player inaccessable or something)
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
                        cout << "mob move mistake " << dir << " " << cx << " " << cy << " " << unit->x << " " << unit->y << endl;
                    }
                }
            } else {
                unit->theTime += 20;
            }
        }
    }
}

void Start::playerWalkStaminaDrain(int* movSpeed, int tim, Unit* unit) {
    if (unit == player->getUnit()) {
        switch(loadStatus) {
            case 0: unit->modifyStat(S_STAMINA, -tim / 2); break;
            case 1: *movSpeed -= 1; unit->modifyStat(S_STAMINA, -tim); break;
            case 2: *movSpeed -= 2; unit->modifyStat(S_STAMINA, -tim * 3 / 2); break;
            case 3: *movSpeed -= 3;   unit->modifyStat(S_STAMINA, -tim * 2); break;
            default: *movSpeed -= 4;  unit->modifyStat(S_STAMINA, -tim * 5 / 2); break;
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
                    attackUnit(unit, zone, dir, true);
                } else {
                    unit->theTime += 2;
                    //makePath(unit, player->getUnit()->x, player->getUnit()->y, zone, true);
                    //if (unit->pointOnPath == -1) {
                        makePath(unit, player->getUnit()->x, player->getUnit()->y, zone, P_NORMAL);
                    //}
                }
                if (unit->pointOnPath > 0) {
                    unit->pointOnPath--;
                }
            } else {
                Location* prevLoc = zone->getLocationAt(x, y);
                int prevHeight = prevLoc->getTotalHeight();
                int height = nextLoc->getTotalHeight();
                if (nextLoc->isOpen()) {
                    if (prevHeight == height) {
                        normalMove = true;
                        changeLoc(unit, zone, newX, newY);
                        int tim;
                        int movSpeed = unit->getStatValue(S_MOVESPEED);
                        if (diags) tim = T_WALK_DI;
                        else tim = T_WALK_ST;
                        if (unit == player->getUnit()) {
                            playerWalkStaminaDrain(&movSpeed, tim, unit);
                        }
                        tim = actionTimePassed(tim, movSpeed);
                        unit->theTime += tim;
                    } else if (fabs(prevHeight - height) <= 2) {
                        normalMove = true;
                        changeLoc(unit, zone, newX, newY);
                        int tim;
                        int movSpeed = unit->getStatValue(S_MOVESPEED) - 1;
                        if (diags) tim = T_WALK_DI;
                        else tim = T_WALK_ST;
                        if (unit == player->getUnit()) {
                            playerWalkStaminaDrain(&movSpeed, tim, unit);
                        }
                        tim = actionTimePassed(tim, movSpeed);
                        unit->theTime += tim;
                    } else {
                        if (prevHeight < height && nextLoc->structure == S_ROCK) {
                            pushRock(unit, zone, dir);
                        } else {
                            openDoor(unit, zone, dir, true);
                        }
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
            action(A_MOVEDIR, unit, dir, x, y);
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
                    action(A_MOVEDIR, unit, dir, unit->x, unit->y);
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

void Start::attackUnit(Unit* unit, Zone* zone, int dir, bool safe) {
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
                Unit* enemy = loc->unit;
                unit->setEnemy(enemy);

                string verb;
                string u1name;
                string u2name;

                float damage = unit->getStatValueF(S_DAMAGE);
                damage *= ((float)rand() / RAND_MAX) / 8 + .9375;
                int hitType;
                float howLuckyAreYou = (float)rand() / RAND_MAX;
                color c;
                if (howLuckyAreYou > unit->getStatValueF(S_MCRITC)) {
                    damage *= 5;
                    verb = "megacrit"; hitType = 4; c = red;
                } else if (howLuckyAreYou > unit->getStatValueF(S_CRITC)) {
                    damage *= 2;
                    verb = "crit";     hitType = 3; c = brick;
                } else if (howLuckyAreYou > unit->getStatValueF(S_HITC)) {
                    verb = "hit";      hitType = 2; c = maroon;
                } else if (howLuckyAreYou > unit->getStatValueF(S_SCRAPEC)) {
                    damage /= 2;
                    verb = "scrape";   hitType = 1; c = dark(maroon);
                } else {
                    damage = 0;
                    verb = "miss";     hitType = 0; c = dark(salmon);
                }

                if (unit == player->getUnit()) {
                    u1name = "you";
                } else {
                    u1name = "the " + unit->name;
                    c.green += c.red / 2;
                    verb = pluralize(verb);
                }
                if (enemy == player->getUnit()) {
                    if (enemy == unit) u2name = "yourself";
                    else u2name = "you";
                } else {
                    if (enemy == unit) u2name = "itself";
                    else u2name = "the " + enemy->name;
                }

                int damageType = -1;
                if (unit == player->getUnit()) {
                    Item* item = primeFolder->getEquips()->getItem(E_RHAND);
                    ItemType* itemType = getItemType(item->itemType);
                    if (itemType->getType() != I_SLOT) {
                        damageType = itemType->getStatValue(S_DTYPE);
                    }
                }
                if (damageType == -1) {
                    damageType = unarmedDamageTypes[unit->getStatValue(S_UNARMED)];
                }
                string extra;
                rAttack(enemy->x, enemy->y, dir, damageType, hitType);

                unit->theTime += actionTimePassed(T_ATTACK, unit->getStatValue(S_MOVESPEED));
                if (unit == player->getUnit()) {
                    unit->modifyStat(S_STAMINA, -T_ATTACK);
                }

                if (damage) {
                    int hp = enemy->modifyStat(S_HP, -(int)damage);
                    if (hp > 0) {
                        int conditionAffectC = unit->getStatValue(S_ATTCONDCHANCE);
                        if (conditionAffectC > 0 && rand() % 100 <= conditionAffectC) {
                            int conditionAffect = unit->getStatValue(S_ATTCOND);
                            if (enemy == player->getUnit()) {
                                switch(conditionAffect) {
                                case C_CONFUSION: extra = " and confuses";
                                addStatus("Confused", maroon, ST_CONF);
                                break;
                                case C_POISON: extra = " and stings"; break;
                                default: break;
                                }
                            }
                            if (conditionAffect >= C_POISON && conditionAffect < C_POISON + 16) {
                                applyPoison(conditionAffect, 10, enemy);
                            } else {
                                enemy->setCondition(conditionAffect, true);
                            }
                        }
                    }
                    int splatterChance;
                    if (hp <= 0) {
                        splatterChance = 10;
                    } else {
                        splatterChance = 10 * damage / hp;
                    }
                    splatterChance *= enemy->getStatValue(S_SPLATTER);
                    if (rand() % 200 < splatterChance) {
                        makeSplatter(enemy, newX, newY);
                    }

                    if (hp <= 0) {
                        if (unit == player->getUnit()) {
                            unit->modifyStat(S_EXP, (enemy->getStatValue(S_LEVEL) + 2) * 4);
                        }
                        killUnit(enemy, zone);
                    }
                }
                addMessage(capitalize(u1name + " " + verb + " " + u2name + extra + "."), c);
            }
        }
    }
}

int poisonWeights[] = {1, 2, 4, 1, 1, 1, 1, 1, 1, 3, 3, 1, 2, 2, 3, 3};
void Start::applyPoison(int condition, int duration, Unit* unit) {
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
        cout << "happens first of all" << endl;
        if (slow || cripple || paralysis || deathly || befuddle) {
            cout << "happens " << endl;
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
                cout << "happens again" << endl;
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
    zone->getLocationAt(unit->x, unit->y)->removeUnit();
    areaUnits.erase(pair<Unit*, Zone*>(unit, zone));
    int split = unit->getStatValue(S_SPLIT);
    if (split) {
        for (int i = 0; i < split; i++) {
            if (spawnMobSpeTag(unit->getStatValue(S_SPAWN), zone, unit->x, unit->y, true, A_MOVELOC)) {

            } else break;
        }
    }
    unitDeleteList.push_back(unit);
    unit->x = -2;
    unit->y = -2;
    if (unit == player->getUnit()) {
        addMessage("GAME OVER HAHA", black);
    }
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
    unit->modifyStat(S_HUNGER, food->getStatValue(S_FEED));
    unit->theTime += T_EAT;
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
    //cout << "capooee " << newZone << endl;
    if (!newZone->isFilled()) {
        generateZone(newZone, 0, newZone->getGenType(), 0, 0, newZone->getWidth(), newZone->getHeight());
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
