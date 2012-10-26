#include <string>
#include <math.h>
#include <cctype>
#include <iostream>
using namespace std;

#ifndef USEFULPARSING_H
#define USEFULPARSING_H

int cti(char c);
int sti(string s);
pair<int, int> stp(string s);
pair<string, string> stps(string s);
float stf(string s);
string its(int num);
string its0(int num);
int numDigits(int num);
int numDigits0(int num);
string capitalize(string s);
string pluralize(string s);
string toRomanNumerals(int num);

#endif // USEFULPARSING_H
