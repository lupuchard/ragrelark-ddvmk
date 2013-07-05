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

#include "Tile.h"

#ifndef ITEMTYPE_H
#define ITEMTYPE_H

#define E_NONE -1
enum {E_HEAD, E_FACE, E_BACK, E_BAG, E_NECK, E_BODY, E_LHAND, E_RHAND, E_HANDS, E_WAIST, E_WRIST, E_FEET, E_RING1, E_RING2, E_AMMO};
#define E_RING 20
#define E_BHANDS 21
#define E_BBHANDS 22
#define E_CG 23
static const int TIME_EQUIP[] = {20, 22, 30, 18, 60, 15, 16, 24, 40, 19, 26, 17, 17, 0, 0, 0, 0, 0, 0, 17, 17, 18, 23};

static const std::string TYPE_NAMES[] = {"none", "anon", "rem", "slot", "", "", "", "", "", "fdr", "ground fdr", "equip fdr", "bag fdr", "prime fdr", "", "", //0-15
    "", "", "", "", "headgear", "face", "back", "bag", "neck", "body", "left hand", "right hand", "hands", "waist", "wrist", "feet",    //16-31
    "ring", "", "", "", "", "", "", "", "", "food", "", "liquid", "", "potion", "", "coin",                                             //32-47
    "", "tool", "", "furniture", "", "ore", "resource", "seed", "", "book", "", "map", "", "monster part", "", "lighter",                    //48-63
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",                                                                     //64-79
    "axe", "mace", "bow", "dagger", "dart", "sling", "crossbow", "spear", "staff", "club", "hammstaer", "large hammer", "knife", "javelin", "trident", "scythe", //80-95
    "whip", "flail", "sword", "greatsword", "gun", "automatic gun", "cordumbed gommer", "icg", "lance", "ammo", "arrow", "bolt", "bullet", "", "hat", "helm", //96-111
    "bandana", "", "", "shield", "spellbook", "orb", "", "", "cape", "", "", "", "", "backpack", "quiver", "",                          //112-127
    "", "", "necklace", "amulet", "", "", "", "gloves", "gauntlets", "mittens", "", "", "belt", "", "", "",                             //128-143
    "", "bracelet", "watch", "", "", "", "shoes", "boots", "", "", "", "clothes", "cloth armor", "light armor", "heavy armor", "",      //144-159
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",/**/ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", //160-191
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",/**/ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", //192-223
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",/**/ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};//224-255
static const int TYPE_SLOTS[] = {E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE, E_NONE,
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
static const int TYPE_STACKS[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   //0-31
    1, 1, 1, 1, 1, 1, 1, 1, 1, 250, 1, 15, 1, 15, 1, 250, 1, 15, 1, 250, 1, 15, 250, 250, 1, 15, 1, 1, 1, 250, 1, 250,        //32-63
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //64-95
    1, 1, 1, 1, 1, 1, 1, 1, 1, 250, 250, 250, 250, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //96-127
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //128-159
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //160-191
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,                         //192-223
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};                        //224-255
static const int TYPE_RANGES[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //32-63
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 106, 0, 0, 105, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, //64-95
0, 0, 0, 0, 108, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, //128-159
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

enum EquipGType{EQG_NONE, EQG_NORM, EQG_SMALL, EQG_TALL, EQG_LONG, EQG_GNORM, EQG_GSMALL, EQG_GTALL, EQG_GLONG};

enum DamType{DAMT_NONE, DAMT_BASH, DAMT_SLASH, DAMT_PIERCE, DAMT_SPELL};
enum           WeapAttType{WEAP_NONE, WEAP_CLAWS, WEAP_TENTACLE, WEAP_FISTS, WEAP_HEAD, WEAP_SLAM, WEAP_BITE, WEAP_CLUB, WEAP_DAGGER, WEAP_AXE, WEAP_SPEAR, WEAP_SCYTHE, WEAP_ARROW, WEAP_STONE, WEAP_OBJ};
static const int WEAP_DAM_TYPES[] = {DAMT_NONE, DAMT_SLASH, DAMT_BASH, DAMT_BASH, DAMT_BASH, DAMT_BASH, DAMT_PIERCE, DAMT_BASH, DAMT_SLASH, DAMT_SLASH, DAMT_PIERCE, DAMT_SLASH, DAMT_PIERCE, DAMT_BASH, DAMT_BASH};

class ItemType: public StatHolder {
    public:
        ItemType(std::string name, std::string desc, int iI, int tI, int type);
        virtual ~ItemType();
        void setEquipGraphic(EquipGType gType, int eqGLoc);
        unsigned char getEquipGType();
        unsigned short getEquipGLoc();
        graphic getGraphic();
        graphic getGraphic(int gStack);
        unsigned char getType();
        std::string getName();
        std::string getDescription();
        void addAbility(unsigned short ability);
        bool hasAbility(unsigned short ability);
        const std::set<unsigned short>::iterator getAbilitiesBegin();
        const std::set<unsigned short>::iterator getAbilitiesEnd();
    protected:
    private:
        std::string name;
        std::string description;
        unsigned char type;
        unsigned char equipGType;
        unsigned short equipGLoc;
        graphic g;
        std::set<unsigned short> abilities;
};

ItemType* getItemType(int itemType);
int addItemType(ItemType* itemType);
void clearItemTypes();

#endif // ITEMTYPE_H
