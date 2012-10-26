#ifndef BAGFOLDER_H
#define BAGFOLDER_H

#include <vector>
#include "ItemFolder.h"
using namespace std;


class BagFolder: public ItemFolder {
    public:
        BagFolder(int size);
        int getNumItems();
        Item* getItem(int index);
        Item* getItems();
        Item removeItem(int index);
        bool addItem(Item* item);
        int getCapacity();
        void setCapacity(int c);
    protected:
    private:
        unsigned short size;
        vector<Item> items;
};

#endif // BAGFOLDER_H
