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

#ifndef MAIN_H
#define MAIN_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>
#include <GL/gl.h>

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <list>
#include <bitset>
#include <deque>
#include <stack>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <yaml-cpp/yaml.h>

typedef std::string String;

#include "usefulParsing.h"

/// The width and height, in pixels, of each tile.
#define TILE_SIZE 32
/// The number of tiles across in a texture.
#define TEX_TILE_WIDTH 16

enum SkillE{SKL_BLADE, SKL_BLUDGEON, SKL_UNARMED, SKL_THROW, SKL_SHOOT, SKL_POLEARM,
            SKL_STEALTH, SKL_DODGE, SKL_SHIELD, SKL_ARMOR, SKL_RESIST, SKL_CONCEN,
            SKL_TRAPS, SKL_SURVIVE, SKL_ATHLETE, SKL_INTIMIDATE, SKL_DIPLOMAT, SKL_IDENTIFY,
            SKL_HOLY, SKL_WIND, SKL_FIRE, SKL_UNHOLY, SKL_EARTH, SKL_WATER};

#endif
