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

#ifndef DATALOADER_H_INCLUDED
#define DATALOADER_H_INCLUDED

#define BETWEEN 0
#define MTG 100
#define TILE 200
#define WORLD 300
#define PLAYER 400
#define FILES 500
#define SPELLS 700
#define AREA 800
#define RESOURCES 900
#define STAIRS 1000
#define MAPSPACE 1100
#define ITEM 1200
#define STATS 1300
#define SKILLS 1350
#define UNIT 1400
#define MAPSTACK 1500
#define CONDITIONS 1650
#define MOBSPAW 1700
#define MOBEQUIPS 1800
#define ITEMSPAW 1850
#define TILEDMAPS 1900
#define TILEDMAPSREFER 1950

#define FIRST_FILE "data/base.rag"

#define DEBUG false
#define ERRCHECK true

int getStructIntFromChar(int c);
int stringToStatus(std::string line);

#endif // DATALOADER_H_INCLUDED
