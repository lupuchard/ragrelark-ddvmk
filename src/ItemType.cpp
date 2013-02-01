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
    equipGType = EQG_NONE;
    type = t;
    makeHashMaps();
}

ItemType::~ItemType() {
    removeHashMaps();
}

graphic ItemType::getGraphic() {
    return g;
}

graphic ItemType::getGraphic(int gStack) {
    int foo = getStatValue(S_GSTACK);
    if (foo == 1) {
        graphic g2 = g;
        if (gStack < 3) {
            return g;
        } else if (gStack < 7) {
            g2.loc += 1;
        } else if (gStack < 16) {
            g2.loc += 2;
        } else if (gStack < 40) {
            g2.loc += 3;
        } else if (gStack < 100) {
            g2.loc += 4;
        } else if (gStack < 250) {
            g2.loc += 5;
        } else {
            g2.loc += 6;
        }
        return g2;
    } else {
        return g;
    }
}

void ItemType::setEquipGraphic(EquipGType gType, int eqGLoc) {
    equipGType = gType;
    equipGLoc = eqGLoc;
}

unsigned char ItemType::getEquipGType() {
    return equipGType;
}
unsigned short ItemType::getEquipGLoc() {
    return equipGLoc;
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

void ItemType::addAbility(unsigned short ability) {
    abilities.insert(ability);
}

bool ItemType::hasAbility(unsigned short ability) {
    return abilities.find(ability) != abilities.end();
}

const set<unsigned short>::iterator ItemType::getAbilitiesBegin() {
    return abilities.begin();
}

const set<unsigned short>::iterator ItemType::getAbilitiesEnd() {
    return abilities.end();
}
