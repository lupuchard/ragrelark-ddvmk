#ifndef ENEMYEQUIPSTATHOLDER_H
#define ENEMYEQUIPSTATHOLDER_H

#include "StatHolder.h"
#include "Unit.h"


class EnemyEquipStatHolder: public StatHolder {
    public:
        EnemyEquipStatHolder();

        short getStatValue(int stat);
        float getStatValueF(int stat);

        void setMobEquips(MobEquips* mobEquips);
    protected:
    private:
        MobEquips* mobEquips;
};

#endif // ENEMYEQUIPSTATHOLDER_H
