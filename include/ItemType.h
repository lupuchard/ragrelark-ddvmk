#include "Tile.h"

#ifndef ITEMTYPE_H
#define ITEMTYPE_H

#define E_NONE -1
#define E_HEAD 0
#define E_FACE 1
#define E_BACK 2
#define E_BAG 3
#define E_NECK 4
#define E_BODY 5
#define E_LHAND 6
#define E_RHAND 7
#define E_HANDS 8
#define E_WAIST 9
#define E_WRIST 10
#define E_FEET 11
#define E_RING1 12
#define E_RING2 13
#define E_RING 20
#define E_BHANDS 21
#define E_BBHANDS 22
#define E_CG 23

const string typeNames[] = {"none", "anon", "", "slot", "", "", "", "", "", "fdr", "ground fdr", "equip fdr", "bag fdr", "prime fdr", "", "", //0-15
    "", "", "", "", "headgear", "face", "back", "bag", "neck", "body", "left hand", "right hand", "hands", "waist", "wrist", "feet",    //16-31
    "ring", "", "", "", "", "", "", "", "", "food", "", "liquid", "", "potion", "", "coin",                                             //32-47
    "", "tool", "", "ammo", "", "ore", "resource", "seed", "", "book", "", "map", "", "monster part", "", "lighter",                    //48-63
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                                     //64-79
    "axe", "mace", "bow", "dagger", "dart", "sling", "crossbow", "spear", "staff", "club", "hammer", "large hammer", "knife", "javelin", "trident", "scythe", //80-95
    "whip", "flail", "sword", "greatsword", "gun", "automatic gun", "cordumned gommer", "icg", "lance", "", "", "", "", "", "hat", "helm", //96-111
    "bandana", "", "", "shield", "spellbook", "orb", "", "", "cape", "", "", "", "", "backpack", "quiver", "",                          //112-127
    "", "", "necklace", "amulet", "", "", "", "gloves", "gauntlets", "mittens", "", "", "belt", "", "", "",                             //128-143
    "", "bracelet", "watch", "", "", "", "shoes", "boots", "", "", "", "clothes", "cloth armor", "light armor", "heavy armor", "",      //144-159
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",/**/ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", //160-191
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",/**/ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", //192-223
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",/**/ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};//224-255
const int typeSlots[] = {E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_HEAD, E_FACE, E_BACK, E_BAG, E_NECK, E_BODY, E_LHAND, E_RHAND, E_HANDS, E_WAIST, E_WRIST, E_FEET,
    E_RING, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_RHAND, E_RHAND, E_BHANDS, E_RHAND, E_RHAND, E_BHANDS, E_BHANDS, E_BBHANDS, E_RHAND, E_RHAND, E_RHAND, E_BHANDS, E_RHAND, E_RHAND, E_BBHANDS, E_BBHANDS,
    E_RHAND, E_RHAND, E_RHAND, E_BHANDS, E_RHAND, E_BBHANDS, E_CG, E_CG, E_BHANDS, E_RHAND, E_RHAND, E_RHAND, E_RHAND, E_RHAND, E_HEAD, E_HEAD,
    E_HEAD, E_HEAD, E_HEAD, E_LHAND, E_LHAND, E_LHAND, E_LHAND, E_LHAND, E_BACK, E_BACK, E_BACK, E_BACK, E_BACK, E_BAG, E_BAG, E_BAG,
    E_BAG, E_BAG, E_NECK, E_NECK, E_NECK, E_NECK, E_NECK, E_HANDS, E_HANDS, E_HANDS, E_HANDS, E_HANDS, E_WAIST, E_WAIST, E_WAIST, E_WAIST,
    E_WAIST, E_WRIST, E_WRIST, E_WRIST, E_WRIST, E_WRIST, E_FEET, E_FEET, E_FEET, E_FEET, E_FEET, E_BODY, E_BODY, E_BODY, E_BODY, E_BODY,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
    E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE};
const int typeStacks[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //0-31
    1, 1, 1, 1, 1, 1, 1, 1, 1, 250, 1, 15, 1, 15, 1, 250, 1, 15, 1, 250, 1, 15, 250, 250, 1, 15, 1, 1, 1, 250, 1, 250,        //32-63
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //64-95
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //96-127
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //128-159
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //160-191
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //192-223
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};                        //224-255

class ItemType: public StatHolder {
    public:
        ItemType(string name, string desc, int iI, int tI, int type);
        virtual ~ItemType();
        graphic getGraphic();
        unsigned char getType();
        string getName();
        string getDescription();
    protected:
    private:
        string name;
        string description;
        unsigned char type;
        graphic g;
};

ItemType* getItemType(int itemType);
int addItemType(ItemType* itemType);
void clearItemTypes();

#endif // ITEMTYPE_H
