#ifndef GROUNDFOLDER_H
#define GROUNDFOLDER_H

#include "ItemFolder.h"
#include "Location.h"

void setEnvironmentManager(EnvironmentManager* ev);

class GroundFolder: public ItemFolder {
    public:
        GroundFolder();
        int getNumItems();
        Item* getItem(int index);
        Item* getItems();
        Item removeItem(int index);
        void setLocation(Zone* z, int x, int y);
        bool addItem(Item* item);
    protected:
    private:
        Zone* zone;
        unsigned short locX;
        unsigned short locY;
        //bool first;
};

#endif // GROUNDFOLDER_H
