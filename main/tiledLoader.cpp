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
#include "tinyxml2/tinyxml2.h"
#include "base64/base64.h"

#define TILE_LAYER_TILEOFF 2177
#define HEIGHT_LAYER_TILEOFF 1
#define STRUCT_LAYER_TILEOFF 1
#define ITEM_LAYER_TILEOFF 129
#define MOB_LAYER_TILEOFF 1153

using namespace tinyxml2;

unsigned* spoon(XMLElement* currentLayer) {
    std::string innerText = currentLayer->FirstChildElement()->GetText();
    boost::algorithm::trim(innerText);
    const std::string &text = base64_decode(innerText);
    unsigned* out = (unsigned*)malloc(text.size());
    memcpy(out, text.c_str(), text.size());
    return out;
}

Zone* Start::loadTileFile(std::string fileName, std::string zoneName) {
    using namespace std;
    cout << " -Loading " << fileName << endl;
    XMLDocument doc;
    int success = doc.LoadFile(fileName.c_str());
	if (success == XML_SUCCESS) {
		//dump_to_stdout(&doc); // defined later in the tutorial
		XMLElement* mapBase = doc.RootElement();
        int width = mapBase->IntAttribute("width");
        int height = mapBase->IntAttribute("height");
        int totes = width * height;

		XMLNode* propertiesBase = mapBase->FirstChild();

        int lightness = 0;
        bool isZone = false;

        XMLElement* currentProperty = propertiesBase->FirstChildElement();
		bool done = false;
		while (!done) {
		    string propertyName = currentProperty->Attribute("name");
		    if (propertyName == "lightness") {
                lightness = currentProperty->IntAttribute("value");
		    } else if (propertyName == "type") {
                isZone = (strcmp(currentProperty->Attribute("value"), "zone") == 0);
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
		if (strcmp(currentLayer->Attribute("name"), "i1") == 0) { //TODO multile layers of items
            unsigned* out = spoon(currentLayer);
            for (int i = 0; i < totes; i++) {
                if (out[i]) {
                    unsigned int index = out[i] - ITEM_LAYER_TILEOFF;
                    addItemToPlace(Coord(i % width, i / width), theZone, tiledItems[index]->genItem());
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
                    mobSpawner->spawnMobSpeTag(tiledMobs[index], theZone, Coord(i % width, i / width), false);
                }
            }
            free(out);
		}
		return theZone;
	}
    cout << "File could not be loadeded.";
    return NULL;
}
