#include "ItemType.h"

vector<ItemType*> itemTypes;

ItemType* getItemType(int itemType) {
    return itemTypes[itemType];
}

int addItemType(ItemType* itemType) {
    itemTypes.push_back(itemType);
    return itemTypes.size() - 1;
}

void clearItemTypes() {
    itemTypes.clear();
}

ItemType::ItemType(string n, string desc, int iI, int tI, int t): StatHolder(V_ITEM) {
    name = n;
    description = desc;
    g.loc = iI;
    g.tex = tI;
    g.type = 0;
    g.border = 0;
    type = t;
    makeHashMaps();
}

ItemType::~ItemType() {
    removeHashMaps();
}

graphic ItemType::getGraphic() {
    return g;
}

unsigned char ItemType::getType() {
    return type;
}

string ItemType::getName() {
    return name;
}

string ItemType::getDescription() {
    return description;
}
