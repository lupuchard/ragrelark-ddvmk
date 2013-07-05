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

/// This is my color class. There are many like it, but this one is mine.

#ifndef COLOR_H
#define COLOR_H

/// Only four bytes instead of however many Color is (16???).
struct MiniColor {
    unsigned char r, g, b, a;
};

/// Hue, saturation and value.
struct HSV {
    float h, s, v;
};

class Color {
    public:
        /// The default color is a fully opaque black. Proven by science.
        Color();

        /// Creates a new Color. Alpha is optional.
        /** All three parameters must range from 0 to 255. */
        Color(int r, int g, int b, int a = 255);

        /// Creates a new Color from that silly struct.
        Color(MiniColor color);

        ///Creates a new Color from HSV values. Alpha is optional.
        /** Note that at least one of the float parameters must explicitly be a float, or it will conflict with the main constructor.
         *  h ranges from 0 to 360
         *  s ranges from 0 to 1
         *  v ranges from 0 to 1
         *  a ranges from 0 to 255
        **/
        Color(float h, float s, float v, int a = 255);

        Color(int gray);


        /// Returns the inverted color.
        Color invert() const;

        /// Returns a darker color.
        Color darken(float factor = 2.f) const;

        /// Returns a lighter color.
        Color lighten(float factor = 2.f) const;

        Color trans(float factor = 2.f) const;
        Color opaq(float factor = 2.f) const;

        /// Returns a gray.
        Color desaturate() const;

        /// Shifts the hue of the color by a number of degrees.
        Color shift(float degrees) const;


        /// Returns a mini color.
        MiniColor mini() const;

        /// Returns the HSV values for this color.
        HSV hsv() const;


        /// Calls glColor for this color.
        void gl() const;


        /// Returns a value as if the Color were an array of [red, green, blue, alpha]. I'm not sure how this would be useful.
        int operator[](int idx) const;


        int red, green, blue, alpha;

        template<class Archive> void serialize(Archive& ar, const unsigned int version) {
            MiniColor c = mini();
            ar & c.r & c.g & c.b & c.a;
            red = c.r; green = c.g; blue = c.b; alpha = c.a;
        }
};

/// Returns true if the colors are the same. (alpha is included in the comparison)
bool operator==(const Color& lhs, const Color& rhs);
/// Returns true if the colors are different. (alpha is included in the comparison)
bool operator!=(const Color& lhs, const Color& rhs);

/// The comparisons compare by intesity (aka the average value of the red, green and blue)
bool operator< (const Color& lhs, const Color& rhs);
bool operator> (const Color& lhs, const Color& rhs);
bool operator<=(const Color& lhs, const Color& rhs);
bool operator>=(const Color& lhs, const Color& rhs);

/// Returns the average of the two colors.
const Color operator+(const Color& rhs, const Color& lhs);

const Color RED        = Color(255,   0,   0); const Color SCARLET    = Color(255,  63,   0); const Color ORANGE     = Color(255, 127,   0);
const Color AMBER      = Color(255, 191,   0); const Color YELLOW     = Color(255, 255,   0); const Color LIME       = Color(191, 255,   0);
const Color CHARTREUSE = Color(127, 255,   0); const Color HARLEQUIN  = Color( 63, 255,   0); const Color GREEN      = Color(  0, 255,   0);
const Color JADE       = Color(  0, 255,  63); const Color SPRING     = Color(  0, 255, 127); const Color TURQUOISE  = Color(  0, 255, 191);
const Color CYAN       = Color(  0, 255, 255); const Color AQUA       = Color(  0, 191, 255); const Color AZURE      = Color(  0, 127, 255);
const Color COBALT     = Color(  0,  63, 255); const Color BLUE       = Color(  0,   0, 255); const Color IRIS       = Color( 63,   0, 255);
const Color VIOLET     = Color(127,   0, 255); const Color AMETHYST   = Color(191,   0, 255); const Color MAGENTA    = Color(255,   0, 255);
const Color FUCHSIA    = Color(255,   0, 191); const Color ROSE       = Color(255,   0, 127); const Color CRIMSON    = Color(255,   0,  63);

const Color MAROON     = Color(127,   0,   0); const Color PINK       = Color(255, 127, 127); const Color SALMON     = Color(191,  63,  63);
const Color OLIVE      = Color(127, 127,   0); const Color CREAM      = Color(255, 255, 127); const Color OLIVINE    = Color(191, 191,  63);
const Color FOREST     = Color(  0, 127,   0); const Color GRASS      = Color(127, 255, 127); const Color FERN       = Color( 63, 191,  63);
const Color TEAL       = Color(  0, 127, 127); const Color CELESTE    = Color(127, 255, 255); const Color AQUAMARINE = Color( 63, 191, 191);
const Color NAVY       = Color(  0,   0, 127); const Color SKY        = Color(127, 127, 255); const Color GLAUCOUS   = Color( 63,  63, 191);
const Color PURPLE     = Color(127,   0, 127); const Color THISTLE    = Color(255, 127, 255); const Color LAVENDER   = Color(191,  63, 191);

const Color BROWN      = Color(127,  63,   0); const Color SWAMP      = Color(  0, 127,  63); const Color INDIGO     = Color( 63,   0, 127);
const Color CORAL      = Color(255, 127,  63); const Color BLELLOW    = Color( 63, 255, 127); const Color SLUE       = Color(127,  63, 255);
const Color TAN        = Color(223, 191, 127); const Color ORCHID     = Color(218, 127, 218); const Color BRICK      = Color(191,   0,   0);
const Color WHITE      = Color(255, 255, 255); const Color SHINE      = Color(223, 223, 223); const Color SILVER     = Color(191, 191, 191); const Color GRAY       = Color(127, 127, 127);
const Color GREY       = Color( 63,  63,  63); const Color CHARCOAL   = Color( 31,  31,  31); const Color TAR        = Color( 15,  15,  15); const Color BLACK      = Color(  0,   0,   0);

/// A fully transparent black.
const Color INVISIBLE  = Color(0, 0, 0, 0);
/// The average color of the universe.
const Color COSMIC_LATTE = Color(255, 248, 231);
/// Real darkness.
const Color EIGENGRAU  = Color( 22,  22,  29);
/// To be used as the occasional parameter. Not equal to zero.
const Color NULL_COLOR = Color(-1, -1, -1, -1);

#endif // COLOR_H
