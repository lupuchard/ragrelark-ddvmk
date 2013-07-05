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

/// Pretty straightforward.

#ifndef COORD_H
#define COORD_H

struct Coord {
    int x, y;

    /// Creates a new Coord at the origin.
    Coord();
    /// Creates a new Coord at that there place.
    Coord(int x, int y);
    /// Copy constructor.
    Coord(const Coord& c);
    /// Haha this deconstructor doesnt do anything.
    ~Coord();

    /// The distance from the origin to the Coord.
    float magnitude();
    /// Rotates the Coord by a number of degrees that's divisible by 90.
    Coord rotate(unsigned int degrees);

    Coord abs();

    bool inBounds(int width, int height);

    int index(int width);

    Coord& operator=(Coord rhs);
    Coord& operator+=(const Coord& rhs);
    Coord& operator-=(const Coord& rhs);
    Coord operator-();
};
inline Coord operator+(Coord lhs, const Coord& rhs) {lhs += rhs; return lhs;}
inline Coord operator-(Coord lhs, const Coord& rhs) {lhs -= rhs; return lhs;}
inline int operator*(const Coord& lhs, const Coord& rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y;
}
inline Coord operator*(const Coord& lhs, const int& rhs) {
    return Coord(lhs.x * rhs, lhs.y * rhs);
}
inline bool operator==(const Coord& lhs, const Coord& rhs) {
    return (lhs.x == rhs.x && lhs.y == rhs.y);
}
inline bool operator!=(const Coord& lhs, const Coord& rhs) {return !operator==(lhs, rhs);}

const Coord ORIGIN(0, 0);

const int NUM_COORD_DIRS = 10;
const Coord DIRS[10] = {Coord(0, 0), Coord(-1, 1), Coord(0, 1), Coord(1, 1), Coord(-1, 0), Coord(0, 0), Coord(1, 0), Coord(-1, -1), Coord(0, -1), Coord(1, -1)};

#endif // COORD_H
