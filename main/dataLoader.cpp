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

#include "Start.h"

/* Loads ALL the data. */
void Start::loadData() {
    using namespace std;

    cout << "Parsing yaml files..." << endl;

    yamlSingle("data/skills.yaml"           , Stat::parseSkills);
    yamlSingle("data/stats.yaml"            , Stat::parseAll);
    yamlSingle("data/prototype_stats.yaml"  , boost::bind(&MobSpawner::parseDefaultStats, mobSpawner, _1));
    yamlSingle("data/spell_elem.yaml"       , Ability::parseElem);
    yamlMulti("data/spells"                 , Ability::parse);
    yamlSingle("data/weap_types.yaml"       , ItemType::parseWeapTypes);
    yamlSingle("data/item_slots.yaml"       , ItemType::parseSlots);
    yamlSingle("data/item_type_types.yaml"  , ItemType::parseTypes);

    primeFolder = new PrimeFolder();
    player = new Player(primeFolder);
    initFieldOfView();

    yamlSingle("data/inventory.yaml"        , boost::bind(&PrimeFolder::parseInv, primeFolder, _1));
    yamlMulti("data/items"                  , ItemType::parse);
    yamlSingle("data/items_tiled.yaml"      , boost::bind(&TiledLoader::parseItems, tiledLoader, _1));
    yamlMulti("data/units"                  , boost::bind(&MobSpawner::parseMob, mobSpawner, _1));
    yamlSingle("data/units_tiled.yaml"      , boost::bind(&TiledLoader::parseUnits, tiledLoader, _1));
    yamlSingle("data/spawn/rarities.yaml"   , boost::bind(&MobSpawner::parseRarities, mobSpawner, _1));
    yamlSingle("data/spawn/mob_equipment.yaml", MobEquipSet::parseAll);
    yamlMulti("data/spawn/items"            , boost::bind(&MobSpawner::parseItems, mobSpawner, _1));
    yamlMulti("data/spawn/mobs"             , boost::bind(&MobSpawner::parseSpawn, mobSpawner, _1));
    yamlMulti("data/tiles"                  , boost::bind(&TiledLoader::parseTiles, tiledLoader, _1));
    yamlSingle("data/tile_groups.yaml"      , Tile::parseSets);
    yamlSingle("data/areas.yaml"            , boost::bind(&World::parseArea, world, _1));
    yamlSingle("data/maps.yaml"             , boost::bind(&World::parseZone, world, _1, tiledLoader, mobSpawner));
    yamlSingle("data/player.yaml"           , boost::bind(&Start::parsePlayer, this, _1));

    cout << "All done with data loading." << endl;
}

void Start::yamlSingle(String file, boost::function<void(YAML::Node)> parseFunc) {
    if (file.substr(file.size() - 4) != "yaml" && file.substr(file.size() - 3) != "yml") return;
    std::cout << "Parsing " << file << ".\n";
    try {
        YAML::Node node = YAML::Load(file);
        std::vector<YAML::Node> itemNodes = YAML::LoadAllFromFile(file);
        for (std::vector<YAML::Node>::iterator iter = itemNodes.begin(); iter != itemNodes.end(); ++iter) {
            parseFunc(*iter);
        }
    } catch(YAML::ParserException& ex) {
        std::cout << ex.what() << "\n";
    }
}

void Start::yamlMulti(String directory, boost::function<void(YAML::Node)> parseFunc) {
    using namespace boost::filesystem;
    path p = directory;
    try {
        if (exists(p) && is_directory(p)) {
            for (directory_iterator iter = directory_iterator(p); iter != directory_iterator(); ++iter) {
                yamlSingle(iter->path().native(), parseFunc);
            }
        } else {
            std::cout << "Error: path not a directory" << std::endl;
        }
    } catch(const filesystem_error& ex) {
        std::cout << ex.what() << "\n";
    }
}

void Start::deleteData() {
    Texture::clear();
    ItemType::clear();
    MobEquipSet::clear();
    Tile::clear();
    Stat::clear();
    glDeleteLists(base, 96);
}
