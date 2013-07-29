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

#include "ItemType.h"
#include "Texture.h"

ItemType::ItemType(String n, String desc, Graphic g, int t): StatHolder(V_ITEM), name(n), description(desc), graphic(g) {
    type = t;
    add(this);
}

ItemType::~ItemType() {}

Graphic ItemType::getGraphic() {
    return graphic;
}

Graphic ItemType::getGraphic(int gStack) {
    int foo = getStatValue(Stat::GSTACK);
    if (foo == 1) {
        Graphic g2 = graphic;
        if (gStack < 3) {
            return graphic;
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
        return graphic;
    }
}

void ItemType::setEquippedGraphic(Graphic g) {
    equipGraphic = g;
}
Graphic ItemType::getEquippedGraphic() {
    return equipGraphic;
}

ItemTypeType* ItemType::getType() {
    return &itemTypeTypes[type];
}

unsigned short ItemType::getIndex() {
    return index;
}

String ItemType::getName() {
    return name;
}

String ItemType::getDescription() {
    return description;
}

void ItemType::addAbility(unsigned short ability) {
    abilities.insert(ability);
}

bool ItemType::hasAbility(unsigned short ability) {
    return abilities.find(ability) != abilities.end();
}

const std::set<unsigned short>::iterator ItemType::getAbilitiesBegin() {
    return abilities.begin();
}

const std::set<unsigned short>::iterator ItemType::getAbilitiesEnd() {
    return abilities.end();
}

ItemSlot* ItemType::getSlot() {
    return itemTypeTypes[type].slot;
}
int ItemType::getStack() {
    return itemTypeTypes[type].stack;
}
bool ItemType::isRanged() {
    return itemTypeTypes[type].ranged;
}
bool ItemType::isFolder() {
    return itemTypeTypes[type].folder;
}
bool ItemType::isSlot() {
    return itemTypeTypes[type].isSlot;
}
bool ItemType::isEdible() {
    return itemTypeTypes[type].edible;
}
ItemTypeType* ItemType::getAmmo() {
    return itemTypeTypes[type].ammo;
}


//statics
std::vector<ItemType*> ItemType::itemTypes;
std::map<String, ItemType*> ItemType::itemTypeNameMap;
ItemTypeType ItemType::itemTypeTypes[256];
std::map<String, int> ItemType::itemTypeTypeNameMap;
int ItemType::numSlots;
std::vector<ItemType*> ItemType::emptySlots;
std::vector<ItemSlot*> ItemType::itemSlots;
std::map<String, ItemSlot*> ItemType::itemSlotNameMap;
std::vector<WeapType*> ItemType::weapTypes;
std::map<String, WeapType*> ItemType::weapTypeNameMap;

struct Prepost{ItemType* itemType; Stat* stat; String name;};
std::list<Prepost> preposts;

void ItemType::parse(YAML::Node fileNode, std::ostream& lerr) {
    String name = readYAMLStr(fileNode, "Name", "nil", "Item lacks a name!", lerr);
    String description = readYAMLStr(fileNode, "Desc", "");
    Graphic g;

    g.tex = Texture::get(readYAMLStr(fileNode, "Texture", "xx", "Item lacks a Texture! (" + name + ")", lerr));
    g.loc = readYAMLCoord(fileNode, "Tile", ORIGIN, "Item lacks a Tile loc! (" + name + ")", lerr).index(TEX_TILE_WIDTH);

    int type;
    String typeStr = readYAMLStr(fileNode, "Type", "", "Item lacks a type! (" + name + ")", lerr);
    if (!typeStr.empty()) {
        if (itemTypeTypeNameMap.find(typeStr) != itemTypeTypeNameMap.end()) {
            type = itemTypeTypeNameMap[typeStr];
        } else {
            lerr << "'" << typeStr << "' is not an existing item type type. (" << name << ")\n";
            type = 0;
        }
    } else type = 0;

    ItemType* newItemType = new ItemType(name, description, g, type);

    YAML::Node eNode = fileNode["Equipped"];
    if (eNode.IsMap()) {
        Graphic equipped;
        String t = readYAMLStr(eNode, "Size", "large", "Item equipped graphic size not specified! (" + name + ")", lerr);
        if (t == "large")      equipped.type = EQG_LARGE;
        else if (t == "long")  equipped.type = EQG_LONG;
        else if (t == "tall")  equipped.type = EQG_TALL;
        else if (t == "small") equipped.type = EQG_SMALL;
        else equipped.type = EQG_NONE;
        equipped.tex = Texture::get(readYAMLStr(eNode, "Texture", "xx", "Item equipped graphic lacks a texture! (" + name + ")", lerr));
        equipped.loc = readYAMLCoord(eNode, "Tile", ORIGIN, "Item equipped graphic lacks a tile loc! (" + name + ")", lerr).index(TEX_TILE_WIDTH * 2);
        newItemType->setEquippedGraphic(equipped);
    }

    YAML::Node sNode = fileNode["Spells"];
    if (sNode.IsSequence()) {
        for (YAML::const_iterator iter = sNode.begin();  iter != sNode.end(); ++iter) {
            String spellName = iter->as<String>();
            if (Ability::has(spellName)) {
                newItemType->addAbility(Ability::get(spellName)->getI());
            } else lerr << "'" << spellName << "' is not an existing spell. (" << name << ")\n";
        }
    }

    for (YAML::Node::iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter) {
        String statStr = iter->first.as<String>();
        if (std::islower(statStr[0]) && !iter->second.IsNull()) {
            if (Stat::has(statStr)) {
                String s = iter->second.as<String>();
                if (isNum(s)) {
                    newItemType->addStatV(Stat::get(statStr)->getIndex(), iter->second.as<int>());
                } else if (isPercent(s)) {
                    newItemType->addStatV(Stat::get(statStr)->getIndex(), stf(s.substr(s.size() - 1)) * 10);
                } else {
                    preposts.push_back(Prepost{newItemType, Stat::get(statStr), s});
                }
            } else {
                lerr << "The stat '" + statStr + "' does not exist! (" << name << ")\n";
            }
        }
    }
}
void ItemType::postParse(std::ostream& lerr) {
    for (std::list<Prepost>::iterator iter = preposts.begin(); iter != preposts.end(); ++iter) {
        if (has(iter->name)) {
            iter->itemType->addStatV(iter->stat->getIndex(), get(iter->name)->getIndex());
        } else lerr << "'" << iter->name << "' is not an existing item or accepted symbol (" << iter->itemType->getName() << ")\n";
    }
}
void ItemType::parseSlots(YAML::Node fileNode, std::ostream& lerr) {
    int index = 0;
    for (YAML::const_iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter) {
        ItemSlot* s = new ItemSlot;
        YAML::Node curNode = *iter;
        s->name = readYAMLStr(curNode, "Name", "");
        s->quantity = readYAMLInt(curNode, "Quantity", 1);
        s->timeEquip = readYAMLInt(curNode, "Time_Equip", 20);
        s->onBody = readYAMLCoord(curNode, "On_Body", ORIGIN);
        if (curNode["Over"]) {
            YAML::Node n = curNode["Over"];
            for (YAML::const_iterator jter = n.begin(); jter != n.end(); ++jter) {
                String nam = jter->as<String>();
                ItemSlot* overSlot = itemSlotNameMap[nam];
                if (jter == n.begin()) s->index = overSlot->index;
                s->over.insert(overSlot);
                overSlot->covers.insert(s);
            }
        } else {
            YAML::Node n = curNode["Tile"];
            Graphic g;
            g.tex = Texture::get(readYAMLStr(curNode, "Texture", "xx", "Item lacks a Texture!", lerr));
            g.loc = readYAMLCoord(curNode, "Tile", ORIGIN, "Slot lacks a tile index!", lerr).index(TEX_TILE_WIDTH);
            ItemType* emptySlot = new ItemType(s->name, "", g, 3);
            for (int i = 0; i < s->quantity; i++) emptySlots.push_back(emptySlot);

            s->index = index;
            index += s->quantity;
        }
        itemSlots.push_back(s);
        itemSlotNameMap[s->name] = s;
    }
    numSlots = index;
}
void ItemType::parseTypes(YAML::Node fileNode, std::ostream& lerr) {
    int i = 0;
    for (YAML::Node::iterator iter = fileNode.begin(); iter != fileNode.end(); ++iter, i++) {
        ItemTypeType type;
        YAML::Node curNode = *iter;
        type.name = readYAMLStr(curNode, "Name", "", "Item type type doesn't have name.", lerr);
        String slot = readYAMLStr(curNode, "Slot", "xx");
        if (slot == "xx") type.slot = NULL;
        else {
            if (itemSlotNameMap.find(slot) == itemSlotNameMap.end()) {
                lerr << "'" << slot << "' is not an existing slot.\n";
                type.slot = NULL;
            } else {
                type.slot = itemSlotNameMap[slot];
            }
        }
        String skillName = readYAMLStr(curNode, "Skill", "xx");
        if (skillName != "xx") {
            if (Stat::hasSkill(skillName)) {
                type.skill = Stat::getSkill(skillName);
            } else lerr << "'" << skillName << "' is not the name of an existing skill.\n";
        } else type.skill = NULL;
        type.stack = readYAMLInt(curNode, "Stack", 1);
        type.ranged = readYAMLInt(curNode, "Ranged", 0);
        type.folder = readYAMLInt(curNode, "Folder", 0);
        type.isSlot = readYAMLInt(curNode, "IsSlot", 0);
        type.edible = readYAMLInt(curNode, "Edible", 0);

        String ammo = readYAMLStr(curNode, "Ammo", "xx");
        if (ammo == "xx") type.ammo = NULL;
        else {
            std::map<String, int>::iterator iter = itemTypeTypeNameMap.find(ammo);
            if (iter != itemTypeTypeNameMap.end()) type.ammo = &itemTypeTypes[iter->second];
            else {
                lerr << "'" << ammo << "' is not an existing ammo.\n";
                type.ammo = NULL;
            }
        }

        String weapType = readYAMLStr(curNode, "Weap_Type", "xx");
        if (weapType == "xx") type.weapType = NULL;
        else {
            std::map<String, WeapType*>::iterator iter = weapTypeNameMap.find(weapType);
            if (iter != weapTypeNameMap.end()) type.weapType = iter->second;
            else {
                lerr << "'" << weapType << "' is not an existing weap type.\n";
                type.weapType = NULL;
            }
        }

        itemTypeTypes[i] = type;
        itemTypeTypeNameMap[type.name] = i;
    }
}
int ItemType::getNumSlots() {
    return numSlots;
}
ItemType* ItemType::getEmptySlot(int index) {
    return emptySlots[index];
}
ItemTypeType* ItemType::getType(int index) {
    return &itemTypeTypes[index];
}
ItemTypeType* ItemType::getType(String s) {
    return &itemTypeTypes[itemTypeTypeNameMap[s]];
}
int ItemType::getTypeI(String s) {
    return itemTypeTypeNameMap[s];
}
bool ItemType::has(String s) {
    return itemTypeNameMap.find(s) != itemTypeNameMap.end();
}
ItemType* ItemType::get(int itemType) {
    return itemTypes[itemType];
}
ItemType* ItemType::get(String s) {
    return itemTypeNameMap[s];
}
int ItemType::add(ItemType* itemType) {
    itemType->index = itemTypes.size();
    itemTypes.push_back(itemType);
    itemTypeNameMap[itemType->name] = itemType;
    return itemType->index;
}
void verbify(YAML::Node node, String& verb, String& verbs, String& default1, String& defaults) {
    if (node.IsSequence()) {
        verb = node[0].as<String>();
        verbs = node[1].as<String>();
    } else if (node) {
        verb = node.as<String>();
        verbs = pluralize(verb);
    } else {
        verb = default1;
        verbs = defaults;
    }
}
void ItemType::parseWeapTypes(YAML::Node node, std::ostream& lerr) {
    bool defaulted = false;
    String scrapeDef, scrapesDef, hitDef, hitsDef, critDef, critsDef, megacritDef, megacritsDef;
    for (YAML::const_iterator iter = node.begin(); iter != node.end(); ++iter) {
        YAML::Node dtypeNode = *iter;
        String name = readYAMLStr(dtypeNode, "Name", "-", "Weap type expects name.", lerr);
        String damName = readYAMLStr(dtypeNode, "Type", "none", "Weap type expects damage type.", lerr);
        DamType type = D_NONE;
        if (damName == "bludgeon") type = D_BLUDGEON;
        else if (damName == "slash") type = D_SLASH;
        else if (damName == "pierce") type = D_PIERCE;
        else if (damName == "spell") type = D_SPELL;
        else if (damName != "none") lerr << "'" << damName << "' is not a damage type." << std::endl;

        String scrape, scrapes, hit, hits, crit, crits, megacrit, megacrits;
        verbify(dtypeNode["Verb_Scrape"]  , scrape  , scrapes  , scrapeDef  , scrapesDef);
        verbify(dtypeNode["Verb_Hit"]     , hit     , hits     , hitDef     , hitsDef);
        verbify(dtypeNode["Verb_Crit"]    , crit    , crits    , critDef    , critsDef);
        verbify(dtypeNode["Verb_Megacrit"], megacrit, megacrits, megacritDef, megacritsDef);
        if (!defaulted) {
            defaulted = true;
            scrapeDef   = scrape;   scrapesDef   = scrapes;
            hitDef      = hit;      hitsDef      = hits;
            critDef     = crit;     critsDef     = crits;
            megacritDef = megacrit; megacritsDef = megacrits;
        }

        WeapType* weapType = new WeapType(type, scrape, scrapes, hit, hits, crit, crits, megacrit, megacrits);
        weapType->index = weapTypes.size();
        weapTypes.push_back(weapType);
        weapTypeNameMap[name] = weapType;
    }
}
bool ItemType::hasWeapType(String type) {
    return weapTypeNameMap.find(type) != weapTypeNameMap.end();
}
WeapType* ItemType::getWeapType(int type) {
    return weapTypes[type];
}
WeapType* ItemType::getWeapType(String type) {
    return weapTypeNameMap[type];
}
void ItemType::clear() {
    itemTypeTypeNameMap.clear();
    for (unsigned int i = 0; i < itemTypes.size(); i++) {
        delete itemTypes[i];
    }
    itemTypes.clear();
    itemTypeNameMap.clear();
    for (unsigned int i = 0; i < itemTypes.size(); i++) {
        delete itemSlots[i];
    }
    itemSlots.clear();
    itemSlotNameMap.clear();
    emptySlots.clear();
    weapTypes.clear();
    weapTypeNameMap.clear();
}
