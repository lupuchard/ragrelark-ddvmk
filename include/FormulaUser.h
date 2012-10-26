#ifndef FORMULAUSER_H
#define FORMULAUSER_H

#include "StatHolderIntef.h"

/* An interface used by Start in order to avoid a circular dependency. foop */
class FormulaUser {
    public:
        FormulaUser();
        virtual ~FormulaUser();

        /* Used by Formula */
        virtual int getVarValue(VOwner target, VType type, int index, StatHolderIntef* sh) = 0;
        virtual float getVarValueF(VOwner target, VType type, int index, StatHolderIntef* sh) = 0;

        /* Used by StatHolders */
        virtual void statChanged(int statI, StatHolderIntef* sh) = 0;
        virtual void conditionChanged(int conditionI, StatHolderIntef* sh) = 0;

        virtual int getTime() = 0;
    protected:
    private:
};

#endif // FORMULAUSER_H
