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
#include "Ability.h"

#ifndef ITEMTYPE_H
#define ITEMTYPE_H

struct ItemSlot {
    String name;
    int index;
    int quantity;
    int timeEquip;
    std::set<ItemSlot*> covers;
    std::set<ItemSlot*> over;
    Coord onBody;
};

struct ItemTypeType {
    String name;
    ItemSlot* slot;
    int stack;
    bool folder;
    bool ranged;
    bool isSlot;
    bool edible;
    ItemTypeType* ammo;
};

enum EquipGType{EQG_NONE, EQG_LARGE, EQG_SMALL, EQG_TALL, EQG_LONG};

enum DamType{DAMT_NONE, DAMT_BASH, DAMT_SLASH, DAMT_PIERCE, DAMT_SPELL};
enum           WeapAttType{WEAP_NONE, WEAP_CLAWS, WEAP_TENTACLE, WEAP_FISTS, WEAP_HEAD, WEAP_SLAM, WEAP_BITE, WEAP_CLUB, WEAP_DAGGER, WEAP_AXE, WEAP_SPEAR, WEAP_SCYTHE, WEAP_ARROW, WEAP_STONE, WEAP_OBJ};
static const int WEAP_DAM_TYPES[] = {DAMT_NONE, DAMT_SLASH, DAMT_BASH, DAMT_BASH, DAMT_BASH, DAMT_BASH, DAMT_PIERCE, DAMT_BASH, DAMT_SLASH, DAMT_SLASH, DAMT_PIERCE, DAMT_SLASH, DAMT_PIERCE, DAMT_BASH, DAMT_BASH};

class ItemType: public StatHolder {
    public:
        ItemType(String name, String desc, Graphic g, int type);
        virtual ~ItemType();

        /// Sets the graphic to be displayed on a person when equipped.
        void setEquippedGraphic(Graphic g);

        Graphic getGraphic();
        Graphic getGraphic(int gStack);
        Graphic getEquippedGraphic();

        ItemTypeType* getType();
        unsigned short getIndex();
        String getName();
        String getDescription();
        void addAbility(unsigned short ability);
        bool hasAbility(unsigned short ability);
        const std::set<unsigned short>::iterator getAbilitiesBegin();
        const std::set<unsigned short>::iterator getAbilitiesEnd();

        ItemSlot* getSlot();
        int getStack();
        bool isRanged();
        bool isFolder();
        bool isSlot();
        bool isEdible();
        ItemTypeType* getAmmo();

        //statics
        static void parse(YAML::Node fileNode);
        static void parseTypes(YAML::Node fileNode);
        static void parseSlots(YAML::Node fileNode);
        static ItemType* get(int index);
        static ItemType* get(String s);
        static bool has(String s);
        static int getTypeI(String s);
        static ItemTypeType* getType(int index);
        static ItemTypeType* getType(String s);
        static int getNumSlots();
        static ItemType* getEmptySlot(int index);
        static void clean();
    private:
        String name;
        String description;
        unsigned char type;
        unsigned short index;

        std::set<unsigned short> abilities;

        Graphic graphic;
        Graphic equipGraphic;

        //statics
        static int add(ItemType* itemType);
        static std::vector<ItemType*> itemTypes;
        static std::map<String, ItemType*> itemTypeNameMap;
        static ItemTypeType itemTypeTypes[256];
        static std::map<String, int> itemTypeTypeNameMap;
        static int numSlots;
        static std::vector<ItemType*> emptySlots;
        static std::vector<ItemSlot*> itemSlots;
        static std::map<String, ItemSlot*> itemSlotNameMap;
};

#endif // ITEMTYPE_H
