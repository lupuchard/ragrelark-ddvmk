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

#include "Coord.h"
#include <algorithm>

Coord::Coord(): x(0), y(0) {}
Coord::Coord(int x, int y): x(x), y(y) {}
Coord::Coord(std::pair<int, int> p): x(p.first), y(p.second) {}
Coord::Coord(const Coord& c): x(c.x), y(c.y) {}
Coord::~Coord() {}

float Coord::magnitude() {
    return std::sqrt(x * x + y * y);
}

Coord Coord::rotate(unsigned int degrees) {
    degrees %= 360;
    switch(degrees) {
        case 0:   return *this;        break;
        case 90:  return Coord(-y, x); break;
        case 180: return -(*this);     break;
        case 270: return Coord(y, -x); break;
        default:  return ORIGIN;
    }
}

int Coord::index(int width) {
    return x + y * width;
}

Coord Coord::abs() {
    return Coord(std::abs(x), std::abs(y));
}

bool Coord::inBounds(int width, int height) {
    return x >= 0 && y >= 0 && x < width && y < height;
}

Coord& Coord::operator=(Coord rhs) {
    this->x = rhs.x;
    this->y = rhs.y;
    return *this;
}

Coord& Coord::operator+=(const Coord& rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
    return *this;
}

Coord& Coord::operator-=(const Coord& rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
    return *this;
}

Coord Coord::operator-() {
    return Coord(-x, -y);
}

Coord& Coord::operator=(std::pair<short, short> p) {
    x = p.first;
    y = p.second;
    return *this;
}

Coord::operator std::pair<short, short>() {
    return std::pair<short, short>(x, y);
}
