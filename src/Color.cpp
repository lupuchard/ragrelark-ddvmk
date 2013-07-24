/*
 *  Copyright 2013 Luke Puchner-Hardman
 *
 *  This file is part of lvp-swarm.
 *  lvp-swarm is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  lvp-swarm is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with lvp-swarm.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <GL/gl.h>
#include "Color.h"
#include <math.h>
#include <algorithm>

using namespace std;

Color::Color(): red(0), green(0), blue(0), alpha(255) {}
Color::Color(int r, int g, int b, int a): red(r), green(g), blue(b), alpha(a) {}
Color::Color(MiniColor c): red(c.r), green(c.g), blue(c.b), alpha(c.a) {}
Color::Color(int gray): red(gray), green(gray), blue(gray), alpha(255) {}

Color::Color(float h, float s, float v, int a) {
    float hue = h / 60.f;
    float chroma = v * s * 255;
    int x = (int)chroma * (1 - fabsf(fmodf(hue, 2) - 1));
    switch((int)hue) {
        case 0: red = chroma;
                green = x;
                blue = 0;
                break;
        case 1: red = x;
                green = chroma;
                blue = 0;
                break;
        case 2: red = 0;
                green = chroma;
                blue = x;
                break;
        case 3: red = 0;
                green = x;
                blue = chroma;
                break;
        case 4: red = x;
                green = 0;
                blue = chroma;
                break;
        case 5: red = chroma;
                green = 0;
                blue = x;
                break;
        default: red = green = blue = 0; break;
    }
    alpha = a;
}

Color Color::invert() const {
    return Color(255 - red, 255 - green, 255 - blue, alpha);
}

Color Color::darken(float factor) const {
    return Color((int)(red / factor), (int)(green / factor), (int)(blue / factor), alpha);
}

Color Color::lighten(float factor) const {
    return Color(255 - (int)((255 - red) / factor), 255 - (int)((255 - green) / factor), 255 - (int)((255 - blue) / factor), alpha);
}

Color Color::trans(float factor) const {
    return Color(red, green, blue, alpha / factor);
}
Color Color::opaq(float factor) const {
    return Color(red, green, blue, 255 - (255 - alpha) / factor);
}

Color Color::desaturate() const {
    float grey = (red + green + blue) / 3;
    return Color(grey, grey, grey, alpha);
}

Color Color::shift(float degrees) const {
    HSV color = hsv();
    return Color(fmodf(color.h + degrees, 60), color.s, color.v, alpha);
}

void Color::gl() const {
    glColor4f(red / 255.f, green / 255.f, blue / 255.f, alpha / 255.f);
}

MiniColor Color::mini() const {
    MiniColor m;
    m.r = red; m.g = green; m.b = blue; m.a = alpha;
    return m;
}

HSV Color::hsv() const {
    HSV color;
    int m = max(red, max(green, blue));
    color.v = m;
    int chroma = m - min(red, min(green, blue));
    if (m == red) {
        color.h = fmodf((float)(green - blue) / chroma, 6) * 60;
    } else if (m == green) {
        color.h = ((float)(blue - red) / chroma + 2) * 60;
    } else if (m == blue) {
        color.h = ((float)(red - green) / chroma + 4) * 60;
    }
    color.s = chroma / color.v;
    return color;
}

int Color::operator[](int idx) const {
    switch(idx) {
        case 0: return red; break;
        case 1: return green; break;
        case 2: return blue; break;
        case 3: return alpha; break;
        default: return -1; break;
    }
}

bool operator==(const Color& lhs, const Color& rhs) {
    return (lhs.red == rhs.red && lhs.green == rhs.green && lhs.blue == rhs.blue && lhs.alpha == rhs.alpha);
}

bool operator!=(const Color& lhs, const Color& rhs) {return !operator==(lhs, rhs);}

bool operator< (const Color& lhs, const Color& rhs) {
    return (lhs.red + lhs.green + lhs.blue) < (rhs.red + rhs.green + rhs.blue);
}
bool operator> (const Color& lhs, const Color& rhs) {return operator<(rhs, lhs);}
bool operator<=(const Color& lhs, const Color& rhs) {return !operator>(lhs, rhs);}
bool operator>=(const Color& lhs, const Color& rhs) {return !operator<(lhs, rhs);}

const Color operator+(const Color& lhs, const Color& rhs) {
    return Color((lhs.red + rhs.red) / 2, (lhs.green + rhs.green) / 2, (lhs.blue + rhs.blue) / 2, (lhs.alpha + rhs.alpha) / 2);
}


