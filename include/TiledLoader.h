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

#ifndef TILEDLOADER_H
#define TILEDLOADER_H

#include "Area.h"
#include "RandItemType.h"

class TiledLoader {
    public:
        TiledLoader(EnvironmentManager* environmentManager, MobSpawner* mobSpawner);
        ~TiledLoader();

        Zone* loadTileFile(String fileName, String zoneName, std::ostream& lerr);

        void parseTiles(YAML::Node node, std::ostream& lerr);
        void parseUnits(YAML::Node node, std::ostream& lerr);
        void parseItems(YAML::Node node, std::ostream& lerr);
    private:
        MobSpawner* mobSpawner;
        EnvironmentManager* enviroManager;

        std::map<unsigned int, Tile*> tiledTiles;
        std::map<unsigned int, RandItemType*> tiledItems;
        std::map<unsigned int, Mob> tiledUnits;

        std::map<String, unsigned int> borderMap;
};

#endif // TILEDLOADER_H
