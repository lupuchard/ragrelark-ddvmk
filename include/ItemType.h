#include "Tile.h"

#ifndef ITEMTYPE_H
#define ITEMTYPE_H

#define E_NONE -1
enum {E_HEAD, E_FACE, E_BACK, E_BAG, E_NECK, E_BODY, E_LHAND, E_RHAND, E_HANDS, E_WAIST, E_WRIST, E_FEET, E_RING1, E_RING2};
#define E_RING 20
#define E_BHANDS 21
#define E_BBHANDS 22
#define E_CG 23
#define E_AMMO 24

const string typeNames[] = {"none", "anon", "", "slot", "", "", "", "", "", "fdr", "ground fdr", "equip fdr", "bag fdr", "prime fdr", "", "", //0-15
    "", "", "", "", "headgear", "face", "back", "bag", "neck", "body", "left hand", "right hand", "hands", "waist", "wrist", "feet",    //16-31
    "ring", "", "", "", "", "", "", "", "", "food", "", "liquid", "", "potion", "", "coin",                                             //32-47
    "", "tool", "", "furniture", "", "ore", "resource", "seed", "", "book", "", "map", "", "monster part", "", "lighter",                    //48-63
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                                     //64-79
    "axe", "mace", "bow", "dagger", "dart", "sling", "crossbow", "spear", "staff", "club", "hammer", "large hammer", "knife", "javelin", "trident", "scythe", //80-95
    "whip", "flail", "sword", "greatsword", "gun", "automatic gun", "cordumned gommer", "icg", "lance", "ammo", "arrow", "bolt", "bullet", "", "hat", "helm", //96-111
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
    E_RHAND, E_RHAND, E_RHAND, E_BHANDS, E_RHAND, E_BBHANDS, E_CG, E_CG, E_BHANDS, E_AMMO, E_AMMO, E_AMMO, E_AMMO, E_AMMO, E_HEAD, E_HEAD,
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

enum EquipGType{EQG_NONE, EQG_NORM, EQG_SMALL, EQG_TALL, EQG_LONG, EQG_GNORM, EQG_GSMALL, EQG_GTALL, EQG_GLONG};

class ItemType: public StatHolder {
    public:
        ItemType(string name, string desc, int iI, int tI, int type);
        virtual ~ItemType();
        void setEquipGraphic(EquipGType gType, int eqGLoc);
        unsigned char getEquipGType();
        unsigned short getEquipGLoc();
        graphic getGraphic();
        graphic getGraphic(int gStack);
        unsigned char getType();
        string getName();
        string getDescription();
    protected:
    private:
        string name;
        string description;
        unsigned char type;
        unsigned char equipGType;
        unsigned short equipGLoc;
        graphic g;
};

ItemType* getItemType(int itemType);
int addItemType(ItemType* itemType);
void clearItemTypes();

#endif // ITEMTYPE_H
