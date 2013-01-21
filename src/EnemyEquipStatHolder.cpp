#include "EnemyEquipStatHolder.h"

EnemyEquipStatHolder::EnemyEquipStatHolder(): StatHolder(V_ITEM) {}

short EnemyEquipStatHolder::getStatValue(int stat) {
    int val = 0;
    for (int i = 0; i < mobEquips->len; i++) {
        val += getItemType(mobEquips->equips[i].itemType)->getStatValue(stat);
    }
    return val;
}

float EnemyEquipStatHolder::getStatValueF(int stat) {
    float val = 0;
    for (int i = 0; i < mobEquips->len; i++) {
        val += getItemType(mobEquips->equips[i].itemType)->getStatValueF(stat);
    }
    return val;
}

void EnemyEquipStatHolder::setMobEquips(MobEquips* mEquips) {
    mobEquips = mEquips;
}
