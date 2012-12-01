#include <string>
#include <math.h>
#include <cctype>
#include <iostream>
using namespace std;

#ifndef USEFULPARSING_H
#define USEFULPARSING_H

/* Converts a char to a 1 digit number. */
int cti(char c);

/* Converts a string to an integer. */
int sti(string s);

/*in: string "X,Y" where X and Y are integers of any length
 *out: a pair of x and y */
pair<int, int> stp(string s);

/*in: string "X:Y"
 *out: pair of strings X and Y*/
pair<string, string> stps(string s);

/* Converts a string to a float. */
float stf(string s);

/* Converts a string to an int. */
string its(int num);
string its0(int num);

/* Returns true if the string is a pair that can be parsed by stp */
bool isPair(string s);

bool isSPair(string s);

/* Returns true if the string only digits and dots. */
bool isNum(string s);

/* Returns the number of digits in a number. */
int numDigits(int num);
int numDigits0(int num);

/* Capitalizes A String */
string capitalize(string s);

/* Pluralizes a strings */
string pluralize(string s);

/* Converts a number to roman numerals. */
string toRomanNumerals(int num);

#endif // USEFULPARSING_H
