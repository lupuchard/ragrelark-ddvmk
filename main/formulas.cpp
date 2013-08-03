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

/*#define F_ITEM 1
#define F_UNIT 2
#define F_ZONE 5
#define F_ENEMY 7*/

int Start::getVarValue(VOwner target, VType type, int index, StatHolderIntef* statHolder) {
    StatHolder* foundStatHolder = findStatHolder(target, static_cast<StatHolder*>(statHolder));
    switch(type) {
        case V_STAT: return foundStatHolder->getStatValue(index); break;
        case V_SKILL: {
            Skill* skill = Stat::getSkill(index);
            if (foundStatHolder == player->getUnit()) return player->getSkillLevel(skill);
            else return 0;
        } break;
        default: break;
    }
    return 0;
}

float Start::getVarValueF(VOwner target, VType type, int index, StatHolderIntef* statHolder) {
    StatHolder* foundStatHolder = findStatHolder(target, static_cast<StatHolder*>(statHolder));
    switch(type) {
        case V_STAT: return foundStatHolder->getStatValue(index); break; //NOTE: UNABLE TO RETRIEVE FLOAT VARIABLES FROM FORMULA AT THIS TIME?
        case V_SKILL: {
            Skill* skill = Stat::getSkill(index);
            if (foundStatHolder == player->getUnit()) return player->getSkillLevel(skill);
            else return 0;
        } break;
        default: break;
    }
    return 0;
}

void Start::statChanged(int stat, StatHolderIntef* statHolder) {
    Stat* theStat = Stat::get(statHolder->getOwner(), stat);
    std::vector<Stat::CompleteStat>* affStats = Stat::getAfflictions(theStat, statHolder->getOwner());
    if (affStats) {
        for (unsigned int i = 0; i < affStats->size(); i++) {
            std::pair<int, Stat*> cStat = (*affStats)[i];
            StatHolder* foundStatHolder = findStatHolder(cStat.first, static_cast<StatHolder*>(statHolder));
            foundStatHolder->needToUpdate(cStat.second->getIndex(), cStat.second->isItFloat());
        }
    }
}

void Start::conditionChanged(int condition, StatHolderIntef* statHolder) {
    std::vector<Stat::CompleteStat>* affStats = Stat::getConAfflictions(condition);
    if (affStats) {
        for (unsigned int i = 0; i < affStats->size(); i++) {
            std::pair<int, Stat*> cStat = (*affStats)[i];
            StatHolder* foundStatHolder = findStatHolder(cStat.first, static_cast<StatHolder*>(statHolder));
            foundStatHolder->needToUpdate(cStat.second->getIndex(), cStat.second->isItFloat());
            //statChanged(cStat.second->getIndex(), statHolder); //not really but sorta
        }
    }
}

StatHolder emptyStatHolder = StatHolder(V_ITEM);
StatHolder enemyEquipStatHolder = EnemyEquipStatHolder();

StatHolder* Start::findStatHolder(int target, StatHolder* statHolderFrom) {
    VOwner owner = statHolderFrom->getOwner(); //step 8
    switch(target) {
        case V_ITEM: {
            switch(owner) {
                case V_UNIT: {
                    if (statHolderFrom == player->getUnit()) {
                        return primeFolder->getEquips();
                    } else {
                        //cout << "*** WARNING: other unit equipment not work yet???? ***" << endl; //TODO this
                        return &emptyStatHolder;
                    }
                } break;
                case V_ITEM: return statHolderFrom; break;
                default: std::cout << "*** WARDING: you are doing it wrong " << target << " " << owner << std::endl; break;
            }
            return 0;
        } break;
        case V_UNIT: { //step 9
            switch(owner) {
                case V_UNIT: return statHolderFrom; break; //step 10
                default: std::cout << "*** WARDING: you are doing it wrong " << target << " " << owner << std::endl; break;
            }
        } break;
        case V_ZONE: {
            switch(owner) {
                case V_ZONE: return statHolderFrom; break;
                default: std::cout << "*** WARDING: you are doing it wrong " << target << " " << owner << std::endl; break;
            }
        } break;
        case V_ENEMY:
            if (owner == V_UNIT) {
                return (static_cast<Unit*>(statHolderFrom)->getEnemy());
            } break;
        default: break;
    }
    return 0;
}

int Start::getTime() {
    return world->theTime;
}
