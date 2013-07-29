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

#include <vector>
#include "Area.h"
#include "Location.h"
#include "StatHolder.h"
#include "DungeonStack.h"
#include "TiledLoader.h"

#ifndef WORLD_H
#define WORLD_H

class World: public StatHolder {
    public:
        World();
        virtual ~World();

        void addArea(Area* a);
        int theTime;

        Area* getArea(int index);
        Area* getArea(String name);
        Zone* getZone(String name);

        void parseArea(YAML::Node fileNode, std::ostream& lerr);
        void parseZone(YAML::Node fileNode, std::ostream& lerr, TiledLoader* tiledLoader, MobSpawner* mobSpawner);
    private:
        std::vector<Area*> areas;
        std::map<String, Area*> areaNameMap;
        std::map<String, Zone*> zoneNameMap;
        std::map<String, DungeonStack*> dunNameMap;
};

#endif // WORLD_H
