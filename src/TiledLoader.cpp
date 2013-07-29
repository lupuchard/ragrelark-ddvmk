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

#include "TiledLoader.h"

#include "tinyxml2/tinyxml2.h"
#include "base64/base64.h"

#define TILE_LAYER_TILEOFF 2177
#define HEIGHT_LAYER_TILEOFF 1
#define STRUCT_LAYER_TILEOFF 1
#define ITEM_LAYER_TILEOFF 129
#define MOB_LAYER_TILEOFF 1153

using namespace tinyxml2;

TiledLoader::TiledLoader(EnvironmentManager* environmentManager, MobSpawner* mobSpawner) {
    enviroManager = environmentManager;
    this->mobSpawner = mobSpawner;
}
TiledLoader::~TiledLoader() {
    for (std::map<unsigned int, RandItemType*>::iterator iter = tiledItems.begin(); iter != tiledItems.end(); ++iter) {
        delete iter->second;
    }
}

unsigned* spoon(XMLElement* currentLayer) {
    String innerText = currentLayer->FirstChildElement()->GetText();
    boost::algorithm::trim(innerText);
    const String &text = base64_decode(innerText);
    unsigned* out = (unsigned*)malloc(text.size());
    memcpy(out, text.c_str(), text.size());
    return out;
}

Zone* TiledLoader::loadTileFile(String fileName, String zoneName, std::ostream& lerr) {
    std::cout << " -Loading " << fileName << std::endl;
    XMLDocument doc;
    int success = doc.LoadFile(fileName.c_str());
    if (success != XML_SUCCESS) success = doc.LoadFile(("resources/" + fileName).c_str());
    if (success != XML_SUCCESS) success = doc.LoadFile(("resources/tilemaps/" + fileName).c_str());
	if (success == XML_SUCCESS) {
		XMLElement* mapBase = doc.RootElement();
        int width = mapBase->IntAttribute("width");
        int height = mapBase->IntAttribute("height");
        int totes = width * height;

		XMLNode* propertiesBase = mapBase->FirstChild();

        int lightness = 0;

        XMLElement* currentProperty = propertiesBase->FirstChildElement();
		bool done = false;
		while (!done) {
		    String propertyName = currentProperty->Attribute("name");
		    if (propertyName == "lightness") {
                lightness = currentProperty->IntAttribute("value");
		    }
		    if (currentProperty == propertiesBase->LastChildElement()) {
                done = true;
		    } else {
                currentProperty = currentProperty->NextSiblingElement();
		    }
		}

        Zone* theZone = NULL;

		XMLElement* currentLayer = mapBase->FirstChildElement("layer");
		if (strcmp(currentLayer->Attribute("name"), "ti") == 0) {
		    unsigned* out = spoon(currentLayer);
		    theZone = new Zone(zoneName, width, height, lightness, true);
            for (int i = 0; i < totes; i++) {
                unsigned int ind = out[i] - TILE_LAYER_TILEOFF;
                theZone->getLocationAt(i)->tile = tiledTiles[ind]->getIndex();
                theZone->getLocationAt(i)->height = 4;
                theZone->getLocationAt(i)->structure = S_NONE;
            }
            free(out);
            currentLayer = currentLayer->NextSiblingElement();
		}
		if (strcmp(currentLayer->Attribute("name"), "ht") == 0) {
            unsigned* out = spoon(currentLayer);
            for (int i = 0; i < totes; i++) {
                unsigned int index = out[i] - HEIGHT_LAYER_TILEOFF;
                theZone->getLocationAt(i)->height = MAX_HEIGHT - ((index / 32) * 8 + (index % 32));
            }
            free(out);
		}
		currentLayer = currentLayer->NextSiblingElement();
		if (strcmp(currentLayer->Attribute("name"), "st") == 0) {
            unsigned* out = spoon(currentLayer);
            for (int i = 0; i < totes; i++) {
                unsigned int index;
                if (!out[i]) index = S_NONE;
                else {
                    index = out[i] - STRUCT_LAYER_TILEOFF - 8;
                    index = ((index / 16) * 8 + (index % 16));
                }
                theZone->getLocationAt(i)->structure = index;
            }
            free(out);
		}
		currentLayer = currentLayer->NextSiblingElement();
		if (strcmp(currentLayer->Attribute("name"), "i1") == 0) { //TODO multiple layers of items
            unsigned* out = spoon(currentLayer);
            for (int i = 0; i < totes; i++) {
                if (out[i]) {
                    unsigned int index = out[i] - ITEM_LAYER_TILEOFF;
                    enviroManager->addItemToPlace(Coord(i % width, i / width), theZone, tiledItems[index]->genItem());
                }
            }
            free(out);
		}
		currentLayer = currentLayer->NextSiblingElement();
		if (strcmp(currentLayer->Attribute("name"), "mb") == 0) {
            unsigned* out = spoon(currentLayer);
            for (int i = 0; i < totes; i++) {
                if (out[i]) {
                    unsigned int index = out[i] - MOB_LAYER_TILEOFF;
                    mobSpawner->spawnMob(tiledUnits[index], theZone, Coord(i % width, i / width), 0, false);
                }
            }
            free(out);
		}
		return theZone;
	}
    lerr << "File could not be found/loaded." << std::endl;
    return NULL;
}

void TiledLoader::parseTiles(YAML::Node node, std::ostream& lerr) {
    String texName = readYAMLStr(node, "Texture", "", "No texture defined!");
    Texture* texture = Texture::get(texName);
    for (YAML::Node::iterator iter = node["Tiles"].begin(); iter != node["Tiles"].end(); ++iter) {
        String tileName = iter->first.as<String>();
        YAML::Node data = iter->second;
        if (data.IsSequence()) {
            Graphic g;
            g.tex = texture;
            g.loc = data[0].as<int>() + data[1].as<int>() * TEX_TILE_WIDTH;
            bool blockM = false;
            bool blockL = false;
            Tile* over = NULL;
            int tloc = -1;
            if (data.size() == 3) {
                YAML::Node exData = data[2];
                if (exData["Bord"]) {
                    String borderN = exData["Bord"].as<String>();
                    if (borderMap.find(borderN) == borderMap.end()) {
                        g.border = borderMap.size() + 1;
                        borderMap[borderN] = g.border;
                    } else {
                        g.border = borderMap[borderN];
                    }
                }
                if (exData["Smooth"]) {
                    String smoothN = exData["Smooth"].as<String>();
                    if (smoothN == "norm") g.type = TT_SMOOTH;
                    else if (smoothN == "hill") g.type = TT_SMOOTHUP;
                    else if (smoothN == "pit") g.type = TT_SMOOTHDOWN;
                }
                blockM = readYAMLInt(exData, "Block", 0);
                blockL = readYAMLInt(exData, "Block_Light", 0);
                if (exData["Over"]) {
                    String overN = exData["Over"].as<String>();
                    if (Tile::has(overN)) {
                        over = Tile::get(overN);
                    } else lerr << "'" << overN << "' is not a existing tile.\n";
                }
                tloc = readYAMLCoord(exData, "Tiled", Coord(1, -1)).index(32);
            }
            Tile* tile = new Tile(tileName, g, blockM, blockL, over);
            Tile::add(tile);
            if (tloc > -1) tiledTiles[tloc] = tile;
        } else lerr << " err: Tiles are mapped to sequences.\n";
    }
}
void TiledLoader::parseUnits(YAML::Node node, std::ostream& lerr) {
    for (YAML::Node::iterator iter = node.begin(); iter != node.end(); ++iter) {
        YAML::Node locNode = iter->first;
        YAML::Node mobNode = iter->second;
        int x = locNode[0].as<int>();
        int y = locNode[1].as<int>();
        int index = x + y * 32;
        String name = mobNode.as<String>();
        if (mobSpawner->mobExists(name)) {
            tiledUnits[index] = mobSpawner->getMob(name);
        } else {
            lerr << "'" + name + "' is not an existing unit.\n";
        }
    }
}
void TiledLoader::parseItems(YAML::Node node, std::ostream& lerr) {
    for (YAML::Node::iterator iter = node.begin(); iter != node.end(); ++iter) {
        YAML::Node locNode = iter->first;
        YAML::Node itemNode = iter->second;
        int x = locNode[0].as<int>();
        int y = locNode[1].as<int>();
        int index = x + y * 32;
        int min, max;
        String name;
        if (itemNode.IsSequence()) {
            name = itemNode[0].as<String>();
            min = indexYAMLInt(itemNode, 1, 1, "");
            max = indexYAMLInt(itemNode, 2, min, "");
        } else {
            name = itemNode.as<String>();
            min = max = 1;
        }
        if (ItemType::has(name)) {
            RandItemType* randItemType = new RandItemType(ItemType::get(name)->getIndex(), min, max, 0);
            tiledItems[index] = randItemType;
        } else {
            lerr << "'" + name + "' is not an existing item.\n";
        }
    }
}
