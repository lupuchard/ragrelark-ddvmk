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

#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>

#define VERSION 0
#define SUBVERSION 1
#define TEST 20145

static const char* SAVE_FILE_NAME = "save.dat";
static const char* SAVE_FILE_TEMP = "savetemp.dat";

/* Loads ALL the data. */
bool Start::loadData() {
    std::cout << "Parsing yaml files..." << std::endl;

    if (loadData1()) return true;
    if (loadData2()) return true;

    std::cout << "All done with data loading." << std::endl;
    return false;
}

bool Start::loadData1() {
    if (yamlSingle("data/skills.yaml"           , Stat::parseSkills)) return true;
    if (yamlSingle("data/stats.yaml"            , Stat::parseAll)) return true;
    if (yamlSingle("data/prototype_stats.yaml"  , Unit::parseDefaultStats)) return true;
    if (yamlSingle("data/spell_elem.yaml"       , Ability::parseElem)) return true;
    if (yamlMulti("data/spells"                 , Ability::parse)) return true;
    if (yamlSingle("data/weap_types.yaml"       , ItemType::parseWeapTypes)) return true;
    if (yamlSingle("data/item_slots.yaml"       , ItemType::parseSlots)) return true;
    if (yamlSingle("data/item_type_types.yaml"  , ItemType::parseTypes)) return true;

    primeFolder = new PrimeFolder();
    player = new Player(primeFolder);
    initFieldOfView();

    if (yamlSingle("data/inventory.yaml"        , boost::bind(&PrimeFolder::parseInv, primeFolder, _1, _2))) return true;
    if (yamlMulti("data/items"                  , ItemType::parse)) return true;
    ItemType::postParse(loadErrStream);
    if (yamlSingle("data/items_tiled.yaml"      , boost::bind(&TiledLoader::parseItems, tiledLoader, _1, _2))) return true;
    if (yamlMulti("data/units"                  , Unit::parseMob)) return true;
    if (yamlSingle("data/units_tiled.yaml"      , boost::bind(&TiledLoader::parseUnits, tiledLoader, _1, _2))) return true;
    if (yamlSingle("data/spawn/rarities.yaml"   , boost::bind(&MobSpawner::parseRarities, mobSpawner, _1, _2))) return true;
    if (yamlSingle("data/spawn/mob_equipment.yaml", MobEquipSet::parseAll)) return true;
    if (yamlMulti("data/spawn/items"            , boost::bind(&MobSpawner::parseItems, mobSpawner, _1, _2))) return true;
    if (yamlMulti("data/spawn/mobs"             , boost::bind(&MobSpawner::parseSpawn, mobSpawner, _1, _2))) return true;
    if (yamlMulti("data/tiles"                  , boost::bind(&TiledLoader::parseTiles, tiledLoader, _1, _2))) return true;
    if (yamlSingle("data/tile_groups.yaml"      , Tile::parseSets)) return true;

    return false;
}

bool Start::loadData2() {
    if (yamlSingle("data/areas.yaml"            , boost::bind(&World::parseArea, world, _1, _2))) return true;
    if (yamlSingle("data/maps.yaml"             , boost::bind(&World::parseZone, world, _1, _2, tiledLoader, mobSpawner))) return true;
    if (yamlSingle("data/player.yaml"           , boost::bind(&Start::parsePlayer, this, _1, _2))) return true;

    return false;
}

bool Start::yamlSingle(String file, boost::function<void(YAML::Node, std::ostream&)> parseFunc) {
    if (file.substr(file.size() - 4) != "yaml" && file.substr(file.size() - 3) != "yml") return false;
    loadStr = "Parsing " + file + ".";
    std::cout << loadStr << std::endl;
    try {
        YAML::Node node = YAML::Load(file);
        std::vector<YAML::Node> itemNodes = YAML::LoadAllFromFile(file);
        for (std::vector<YAML::Node>::iterator iter = itemNodes.begin(); iter != itemNodes.end(); ++iter) {
            parseFunc(*iter, loadErrStream);
            if (!loadErrStream.str().empty()) {
                std::cerr << loadErrStream.str() << "  Aborting!" << std::endl;
                return true;
            }
        }
    } catch(YAML::ParserException& ex) {
        std::cout << ex.what() << "\n";
    }
    render();
    return false;
}

bool Start::yamlMulti(String directory, boost::function<void(YAML::Node, std::ostream&)> parseFunc) {
    using namespace boost::filesystem;
    path p = directory;
    try {
        if (exists(p) && is_directory(p)) {
            std::set<String> directories; // i want the directories to have a consistant order in loading
            for (directory_iterator iter = directory_iterator(p); iter != directory_iterator(); ++iter) {
                directories.insert(iter->path().native());
            }
            for (std::set<String>::iterator iter = directories.begin(); iter != directories.end(); ++iter) {
                if (yamlSingle(*iter, parseFunc)) return true;
            }
        } else {
            std::cout << "Error: path not a directory" << std::endl;
        }
    } catch(const filesystem_error& ex) {
        std::cout << ex.what() << "\n";
    }
    return false;
}

void Start::deleteData() {
    Texture::clear();
    ItemType::clear();
    MobEquipSet::clear();
    Tile::clear();
    Stat::clear();
    glDeleteLists(base, 96);
}

void Start::save() {
    std::ofstream tempFile(SAVE_FILE_TEMP, std::ios_base::out | std::ios_base::binary);
    outInt(VERSION   , tempFile);
    outInt(SUBVERSION, tempFile);
    outInt(TEST      , tempFile);

    std::cout << "Saving world..." << std::endl;
    world->save(tempFile);
    int siz = tempFile.tellp();
    std::cout << "(World was " << siz << " bytes)\n";

    std::cout << "Saving you..." << std::endl;
    player->save(tempFile);
    siz = (int)tempFile.tellp() - siz;
    std::cout << "(Player was " << siz << " bytes)\n";

    tempFile.close();

    std::cout << "Compressing..." << std::endl;
    std::remove(SAVE_FILE_NAME);
    std::ofstream saveFile(SAVE_FILE_NAME, std::ios_base::out | std::ios_base::binary);
    std::ifstream fromTemp(SAVE_FILE_TEMP, std::ios_base::in  | std::ios_base::binary);

    using namespace boost::iostreams;
    filtering_streambuf<output> out;
    out.push(zlib_compressor());
    out.push(saveFile);
    copy(fromTemp, out);

    std::cout << "Size of file: " << (int)saveFile.tellp() << " bytes.\n";
    saveFile.close();
    std::remove(SAVE_FILE_TEMP);

    std::cout << "The world is safe once again." << std::endl;
}

bool Start::load() {
    std::ifstream saveFile(SAVE_FILE_NAME, std::ios_base::in  | std::ios_base::binary);
    std::ofstream tempFile(SAVE_FILE_TEMP, std::ios_base::out | std::ios_base::binary);

    loadStr = "Decompressing save...";
    using namespace boost::iostreams;
    filtering_streambuf<input> in;
    in.push(zlib_decompressor());
    in.push(saveFile);
    copy(in, tempFile);

    saveFile.close();
    tempFile.close();

    std::ifstream tempRead(SAVE_FILE_TEMP, std::ios_base::in  | std::ios_base::binary);

    int version = inInt(tempRead);
    int subversion = inInt(tempRead);
    int test = inInt(tempRead);
    if (test != TEST) {
        std::cout << "There is something wrong with the savefile." << std::endl;
        std::remove(SAVE_FILE_TEMP);
        return true;
    } else if (version != VERSION || subversion != SUBVERSION) {
        std::cout << "The save file is not up to date (" << version << "." << subversion << " when it should be " << VERSION << "." << SUBVERSION << ")" << std::endl;
        std::remove(SAVE_FILE_TEMP);
        return true;
    }

    loadStr = "Loading world..."; render();
    world->load(tempRead);
    loadStr = "Loading player..."; render();
    player->load(tempRead, world);
    loadStr = "Done loading."; render();

    tempRead.close();
    std::remove(SAVE_FILE_TEMP);

    return false;
}
