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

#include "World.h"

World::World() {
    theTime = 0;
}

World::~World() {
    for (unsigned int i = 0; i < areas.size(); i++) {
        delete areas[i];
    }
}

void World::addArea(Area* a) {
    areas.push_back(a);
}

Area* World::getArea(int index) {
    return areas[index];
}

Area* World::getArea(String name) {
    return areaNameMap[name];
}

Zone* World::getZone(String name) {
    return zoneNameMap[name];
}

void World::parseArea(YAML::Node fileNode, std::ostream& lerr) {
    String name = readYAMLStr(fileNode, "Name", "nil", "Name expected.", lerr);
    int width = fileNode["Dim"][0].as<int>();
    Area* newArea = new Area(width, name);
    addArea(newArea);
    areaNameMap[name] = newArea;
}

void World::parseZone(YAML::Node fileNode, std::ostream& lerr, TiledLoader* tiledLoader, MobSpawner* mobSpawner) {
    String areaName = readYAMLStr(fileNode, "Area", "nil", "Area expected.", lerr);
    if (areaNameMap.find(areaName) != areaNameMap.end()) {
        String name = readYAMLStr(fileNode, "Name", "nil", "Name expected.", lerr);
        Area* area = areaNameMap[areaName];
        Zone* newZone = NULL;
        if (fileNode["Stack"]) {
            YAML::Node stackNode = fileNode["Stack"];

            String genName = readYAMLStr(fileNode, "Gen", "dungeon");
            GenType gen = GEN_DUNGEON;
            if (genName == "dungeon") gen = GEN_DUNGEON;
            int depth = readYAMLInt(stackNode, "Num_Floors", 10);
            DungeonStack* dungeonStack = new DungeonStack(name, depth, gen);

            String tilesetName = readYAMLStr(stackNode, "Tileset", "nil", "Tileset expected.", lerr);
            if (Tile::hasSet(tilesetName)) {
                dungeonStack->setTileset(Tile::getSet(tilesetName));
            } else lerr << "'" << tilesetName << "' is not an existing tileset.\n";

            dungeonStack->setLight(readYAMLInt(stackNode, "Light", 8), readYAMLNum(stackNode, "LightMod", 0));
            dungeonStack->setNumStairs(readYAMLInt(stackNode, "Stairs", 1), readYAMLNum(stackNode, "StairsMod", 0));
            dungeonStack->setDifficulty(readYAMLInt(stackNode, "Diff", 0), readYAMLNum(stackNode, "DiffMod", 0));

            if (stackNode["Spawn"].IsSequence()) {
                YAML::Node spawnNode = stackNode["Spawn"];
                for (YAML::const_iterator iter = spawnNode.begin(); iter != spawnNode.end(); ++iter) {
                    dungeonStack->addEnvironment(mobSpawner->getEnvironment(iter->as<String>()));
                }
            }
            dungeonStack->createZones();
            newZone = dungeonStack->getZone(0);
            area->addDungeonStack(dungeonStack);
            dunNameMap[name] = dungeonStack;
        } else if (fileNode["File"]) {
            newZone = tiledLoader->loadTileFile(fileNode["File"].as<String>(), name, lerr);
            area->addZone(newZone);
            zoneNameMap[name] = newZone;
        }

        if (newZone && fileNode["Connections"]) {
            YAML::Node cNode = fileNode["Connections"];
            for (YAML::const_iterator iter = cNode.begin(); iter != cNode.end(); ++iter) {
                YAML::Node curNode = *iter;
                if (curNode["From"] && curNode["To"]) {
                    YAML::Node posNode = curNode["From"];
                    Coord pos = Coord(posNode[0].as<int>(), posNode[1].as<int>());
                    String zoneName = curNode["To"].as<String>();
                    if (zoneNameMap.find(zoneName) != zoneNameMap.end()) {
                        area->addConnection(Connection{pos, pos, newZone, zoneNameMap[zoneName]});
                    } else if (dunNameMap.find(zoneName) != dunNameMap.end()) {
                        DungeonStack* ds = dunNameMap[zoneName];
                        area->addConnection(Connection{pos, ds->addEntryStairs(), newZone, ds->getZone(0)});
                    } else lerr << "'" << name << "' is not the name of an existing zone or dungeon stack.\n";
                } else {
                    lerr << "expected From and To in the connection" << std::endl;
                }
            }
        }
    } else lerr << "Area does not exist: " << areaName << "\n";
}

void World::save(std::ostream& saveData) {
    outInt(theTime, saveData);

    outSht(areas.size(), saveData);
    for (unsigned int i = 0; i < areas.size(); i++) {
        areas[i]->save(saveData);
    }
}

void World::load(std::istream& saveData) {
    theTime = inInt(saveData);

    unsigned int numAreas = inSht(saveData);
    for (unsigned int i = 0; i < numAreas; i++) {
        Area* area = new Area(saveData);
        areas.push_back(area);
        areaNameMap[area->getName()] = area;
    }
}
