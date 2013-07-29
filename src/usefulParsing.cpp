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

#include "usefulParsing.h"

using namespace std;

/* Converts a char to a 1 digit number. */
int cti(char c) {
    return c - 48;
}

/* Converts a string to an integer. */
int sti(string s) {
    if (s[0] == '-') {
        return -sti(s.substr(1, 100));
    }
    int num = 0;
    for (unsigned int i = 0; i < s.size(); i++) {
        num *= 10;
        num += cti(s[i]);
    }
    return num;
}

/*in: string "X,Y" where X and Y are integers of any length
 *out: a pair of x and y */
pair<int, int> stp(string s) {
    for (unsigned int i = 0; i < s.size(); i++) {
        if (s[i] == ','  || s[i] == ':' || (i > 0 && s[i] == '-')) {
            return pair<int, int>(sti(s.substr(0, i)), sti(s.substr(i + 1, 100)));
        }
    }
    return pair<int, int>(0, 0);
}

Coord stc(string s) {
    for (unsigned int i = 0; i < s.size(); i++) {
        if (s[i] == ','  || s[i] == ':') {
            return Coord(sti(s.substr(0, i)), sti(s.substr(i + 1, 100)));
        }
    }
    return Coord();
}

bool isPair(string s) {
    bool comma = false;
    for (unsigned int i = 0; i < s.size(); i++) {
        if (s[i] == ',' || s[i] == ':' || (i > 0 && s[i] == '-')) {
            if (comma) {
                return false;
            } else {
                comma = true;
            }
        } else if (s[i] < 48 || s[i] >= 58) {
            return false;
        }
    }
    return comma;
}

bool isSPair(string s) {
    bool comma = false;
    for (unsigned int i = 0; i < s.size(); i++) {
        if (s[i] == ',' || s[i] == ':') {
            if (comma) {
                return false;
            } else {
                comma = true;
            }
        }
    }
    return true;
}

bool isPercent(string s) {
    return isNum(s.substr(0, s.size() - 1)) && s[s.size() - 1] == '%';
}
bool isNum(string s) {
    if ((s[0] < 48 || s[0] >= 58) && s[0] != '.' && s[0] != '-') {
        return false;
    }
    for (unsigned int i = 1; i < s.size(); i++) {
        if ((s[i] < 48 || s[i] >= 58) && (s[i] != '.')) {
            return false;
        }
    }
    return true;
}

/*in: string "X:Y"
 *out: pair of strings X and Y*/
pair<string, string> stps(string s) {
    for (unsigned int i = 0; i < s.size(); i++) {
        if (s[i] == ','  || s[i] == ':') {
            return pair<string, string>(s.substr(0, i), s.substr(i + 1, 100));
        }
    }
    return pair<string, string>(s, "");
}

/* Converts a string to a float. */
float stf(string s) {
    if (s[0] == '-') {
        return -stf(s.substr(1, 100));
    }
    bool dotted = false;
    int downed = 1;
    float num = 0;
    for (unsigned int i = 0; i < s.size(); i++) {
        if (dotted) {
            num += cti(s[i]) * pow(0.1, downed);
            downed++;
        } else if (s[i] == '.') {
            dotted = true;
        } else {
            num *= 10;
            num += cti(s[i]);
        }
    }
    return num;
}

string its(int num) {
    if (num == 0) return "0";
    int len = numDigits(num);
    char cstr[len];
    int i = 0;
    if (num < 0) {
        cstr[0] = '-';
        num = -num;
    }
    while(num) {
        cstr[len - i - 1] = (char)(num % 10 + 48);
        num /= 10;
        i++;
    }
    return string(cstr, len);
}

string its0(int num) {
    if (num == 0) return "0";
    int len = numDigits(num);
    if (num >= 0) {
        len++;
    }
    char cstr[len];
    int i = 0;
    if (num < 0) {
        cstr[0] = '-';
        num = -num;
    } else {
        cstr[0] = '+';
    }
    while(num) {
        cstr[len - i - 1] = (char)(num % 10 + 48);
        num /= 10;
        i++;
    }
    return string(cstr, len);
}

int numDigits(int num) {
    int digits = 0;
    if (num < 0) digits = 1;
    while (num) {
        num /= 10;
        digits++;
    }
    return digits;
}

int numDigits0(int num) {
    int digits = 0;
    while (num) {
        num /= 10;
        digits++;
    }
    return digits;
}

/* Returns the string capitalized. */
string capitalize(string s) {
    s[0] = toupper((unsigned char)s[0]);
    return s;
}

string pluralize(string s) {
    switch(s[s.size() - 1]) {
        case 'e' : {
            if (s[s.size() - 2] == 'f') {
                return s.substr(0, s.size() - 2) + "ves";
            } else {
                return s + "s";
            }
        } break;
        case 'f' : {
            if (s[s.size() - 2] == 'f') {
                return s.substr(0, s.size() - 2) + "ves";
            } else {
                return s.substr(0, s.size() - 1) + "ves";
            }
        } break;
        case 'h' : {
            char c = s[s.size() - 2];
            if (c == 'c'  || c == 's') {
                return s + "es";
            } else {
                return s + "s";
            }
        } break;
        case 'n' : {
            if (s[s.size() - 2] == 'o') {
                return s.substr(0, s.size() - 2) + "a";
            } else {
                return s + "s";
            }
        } break;
        case 'o' : return s + "es"; break;
        case 's' : {
            if (s[s.size() - 2] == 'u') {
                return s.substr(0, s.size() - 2) + "odes";
            } else {
                return s + "es";
            }
        } break;
        case 'x' : return s + "es"; break;
        case 'y' : {
            char c = s[s.size() - 2];
            if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u') {
                return s + "s";
            } else {
                return s.substr(0, s.size() - 1) + "ies";
            }
        } break;
        case 'z' : return s + "es"; break;
        default: return s + "s"; break;
    }
}

String article(String s) {
    if (s[0] == 'a' || s[0] == 'e' || s[0] == 'i' || s[0] == 'o' || s[0] == 'u') return "an";
    return "a";
}

string romanNumerals[4][10] = {{"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"},
                         {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"},
                         {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"},
                         {"", "M", "MM", "MMM", "MMMM", "MMMMM", "MMMMMM", "MMMMMMM", "MMMMMMMM", "MMMMMMMMM"}};
string toRomanNumerals(int num) {
    string s;
    int digit = 0;
    while (num) {
        s += romanNumerals[digit][num % 10];
        num /= 10;
        digit++;
    }
    return s;
}

String readYAMLStr(YAML::Node& node, String key, String def, String errorMess, std::ostream& lerr) {
    YAML::Node n = node[key];
    if (n) {
        return n.as<String>();
    } else {
        if (!errorMess.empty()) lerr << " err: " << errorMess << "\n";
        return def;
    }
}
int readYAMLInt(YAML::Node& node, String key, int def, String errorMess, std::ostream& lerr) {
    YAML::Node n = node[key];
    if (n) {
        String s = n.as<String>();
        if (s == "yes") return 1;
        else if (s == "no") return 0;
        return node[key].as<int>();
    } else {
        if (!errorMess.empty()) lerr << " err: " << errorMess << "\n";
        return def;
    }
}
float readYAMLNum(YAML::Node& node, String key, float def, String errorMess, std::ostream& lerr) {
    YAML::Node n = node[key];
    if (n) {
        return n.as<float>();
    } else {
        if (!errorMess.empty()) lerr << " err: " << errorMess << "\n";
        return def;
    }
}
Coord readYAMLCoord(YAML::Node& node, String key, Coord def, String errorMess, std::ostream& lerr) {
    YAML::Node n = node[key];
    if (n.IsSequence() && n.size() == 2) {
        return Coord(n[0].as<int>(), n[1].as<int>());
    } else {
        if (!errorMess.empty()) lerr << " err: " << errorMess << "\n";
        return def;
    }
}
int indexYAMLInt(YAML::Node& node, int key, int def, String errorMess, std::ostream& lerr) {
    YAML::Node n = node[key];
    if (n) {
        return n.as<int>();
    } else {
        if (!errorMess.empty()) lerr << " err: " << errorMess << "\n";
        return def;
    }
}
