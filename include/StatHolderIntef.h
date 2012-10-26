#ifndef STATHOLDERINTEF_H
#define STATHOLDERINTEF_H

enum VOwner{V_WORLD, V_ITEM, V_UNIT, V_TILE, V_ZONE, V_AREA, V_ENEMY};
enum VType{V_STAT, V_SKILL, V_CONDITION};

class StatHolderIntef {
    public:
        StatHolderIntef();
        virtual ~StatHolderIntef();
        virtual VOwner getOwner() = 0;
    protected:
    private:
};

#endif // STATHOLDERINTEF_H
