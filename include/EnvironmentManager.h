#ifndef ENVIRONMENTMANAGER_H
#define ENVIRONMENTMANAGER_H

#include "Zone.h"

class EnvironmentManager {
    public:
        EnvironmentManager();
        virtual ~EnvironmentManager();
        virtual void addItemToPlace(int x, int y, Zone* z, Item item) = 0;
        virtual Item removeItemFromPlace(int x, int y, Zone* z, int index) = 0;
    protected:
    private:
};

#endif // ENVIRONMENTMANAGER_H
