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

#ifndef USEFULPARSING_H
#define USEFULPARSING_H

#include <string>
#include <math.h>
#include <cctype>
#include <iostream>

#include <yaml-cpp/yaml.h>

#include "Coord.h"

typedef std::string String;

/// Converts a char to a 1 digit number.
int cti(char c);

/// Converts a string to an integer.
int sti(String s);

/**in: string "X,Y" where X and Y are integers of any length
 *out: a pair of x and y */
std::pair<int, int> stp(String s);

/**in: string "X,Y" where X and Y are integers of any length
 *out: a Coord of x and y */
Coord stc(String s);

/**in: string "X:Y"
 *out: pair of strings X and Y*/
std::pair<String, String> stps(String s);

/// Converts a string to a float.
float stf(String s);

/// Converts a string to an int.
String its(int num);
String its0(int num);

/// Returns true if the string is a pair that can be parsed by stp
bool isPair(String s);

bool isSPair(String s);

/// Returns true if the string only digits and dots.
bool isNum(String s);

/// Returns the number of digits in a number.
int numDigits(int num);
int numDigits0(int num);

/// Capitalizes A String
String capitalize(String s);

/// Pluralizes a strings
String pluralize(String s);

String article(String s);

/// Converts a number to roman numerals.
String toRomanNumerals(int num);

String readYAMLStr(  YAML::Node& node, String key, String def, String errorMess = "");
int    readYAMLInt(  YAML::Node& node, String key,    int def, String errorMess = "");
float  readYAMLNum(  YAML::Node& node, String key,  float def, String errorMess = "");
Coord  readYAMLCoord(YAML::Node& node, String key,  Coord def, String errorMess = "");
String indexYAMLStr( YAML::Node& node,    int key, String def, String errorMess = "");
int    indexYAMLInt( YAML::Node& node,    int key,    int def, String errorMess = "");

#endif // USEFULPARSING_H
