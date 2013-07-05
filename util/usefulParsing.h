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

#include <string>
#include <math.h>
#include <cctype>
#include <iostream>

#include "Coord.h"

#ifndef USEFULPARSING_H
#define USEFULPARSING_H

/* Converts a char to a 1 digit number. */
int cti(char c);

/* Converts a string to an integer. */
int sti(std::string s);

/*in: string "X,Y" where X and Y are integers of any length
 *out: a pair of x and y */
std::pair<int, int> stp(std::string s);

/*in: string "X,Y" where X and Y are integers of any length
 *out: a Coord of x and y */
Coord stc(std::string s);

/*in: string "X:Y"
 *out: pair of strings X and Y*/
std::pair<std::string, std::string> stps(std::string s);

/* Converts a string to a float. */
float stf(std::string s);

/* Converts a string to an int. */
std::string its(int num);
std::string its0(int num);

/* Returns true if the string is a pair that can be parsed by stp */
bool isPair(std::string s);

bool isSPair(std::string s);

/* Returns true if the string only digits and dots. */
bool isNum(std::string s);

/* Returns the number of digits in a number. */
int numDigits(int num);
int numDigits0(int num);

/* Capitalizes A String */
std::string capitalize(std::string s);

/* Pluralizes a strings */
std::string pluralize(std::string s);

/* Converts a number to roman numerals. */
std::string toRomanNumerals(int num);

#endif // USEFULPARSING_H
