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
#include "StatHolder.h"
#include "Ability.h"

#ifndef ITEMTYPE_H
#define ITEMTYPE_H

enum DamType{D_NONE, D_BLUDGEON, D_SLASH, D_PIERCE, D_SPELL};
struct WeapType {
    DamType damageType;
    unsigned int index;
    String scrape, scrapes, hit, hits, crit, crits, megacrit, megacrits;
    WeapType(DamType damType, String scrape, String scrapes, String hit, String hits, String crit, String crits, String megacrit, String megacrits):
        damageType(damType), index(-1), scrape(scrape), scrapes(scrapes), hit(hit), hits(hits), crit(crit), crits(crits), megacrit(megacrit), megacrits(megacrits) {}
    String getVerb(int criticality, bool plural) {
        String verb;
        switch(criticality) {
            case 0: verb = plural ? "misses" : "miss";   break;
            case 1: verb = plural ? scrapes  : scrape;   break;
            case 2: verb = plural ? hits     : hit;      break;
            case 3: verb = plural ? crits    : crit;     break;
            case 4: verb = plural ? megacrits: megacrit; break;
            default: verb = plural ? "BUGS"  : "BUG";    break;
        }
        return verb;
    }
};

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
    WeapType* weapType;
    Skill* skill;
    int stack;
    bool folder;
    bool ranged;
    bool isSlot;
    bool edible;
    ItemTypeType* ammo;
};

enum EquipGType{EQG_NONE, EQG_LARGE, EQG_SMALL, EQG_TALL, EQG_LONG};

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
        static void parse(YAML::Node fileNode, std::ostream& lerr);
        static void postParse(std::ostream& lerr);
        static bool has(String s);
        static ItemType* get(int index);
        static ItemType* get(String s);

        static void parseTypes(YAML::Node fileNode, std::ostream& lerr);
        static int getTypeI(String s);
        static ItemTypeType* getType(int index);
        static ItemTypeType* getType(String s);

        static void parseSlots(YAML::Node fileNode, std::ostream& lerr);
        static int getNumSlots();
        static ItemType* getEmptySlot(int index);

        static void parseWeapTypes(YAML::Node node, std::ostream& lerr);
        static bool hasWeapType(String type);
        static WeapType* getWeapType(int type);
        static WeapType* getWeapType(String type);

        static void clear();
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
        static std::vector<WeapType*> weapTypes;
        static std::map<String, WeapType*> weapTypeNameMap;
};

#endif // ITEMTYPE_H
