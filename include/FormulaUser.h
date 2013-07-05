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
