#ifndef ITEMFOLDER_H
#define ITEMFOLDER_H

#include "Item.h"
#include "EnvironmentManager.h"

class ItemFolder {
    public:
        ItemFolder();
        virtual ~ItemFolder();
        virtual int getNumItems() = 0;
        virtual Item* getItem(int index) = 0;
        virtual Item* getItems() = 0;
        virtual Item removeItem(int index) = 0;
        virtual bool addItem(Item* item) = 0;
    protected:
    private:
};

#endif // ITEMFOLDER_H
