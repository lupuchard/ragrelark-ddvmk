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

#include "math.h"
#include "Blobber.h"

#define SIZZ 64

Blobber::Blobber() {
    blobArr = new bool*[SIZZ];
    for(int i = 0; i < SIZZ; i++) {
        blobArr[i] = new bool[SIZZ];
    }
    blobWidth = 0;
    blobHeight = 0;
}

Blobber::~Blobber() {
    for (int i = 0; i < SIZZ; i++) {
        delete[] blobArr[i];
    }
    delete[] blobArr;
}

void Blobber::makeCircle(int rad) {
    blobHeight = blobWidth = rad * 2;
    for (int i = 0; i < rad; i++) {
        for (int j = 0; j < rad; j++) {
            int x = rad - i;
            int y = rad - j;
            double dist = sqrt(x * x + y * y);
            if (dist <= rad - .5) {
                blobArr[i][j] = true;
                blobArr[i][blobHeight - j - 1] = true;
                blobArr[blobWidth - i - 1][j] = true;
                blobArr[blobWidth - i - 1][blobHeight - j - 1] = true;
            } else {
                blobArr[i][j] = false;
                blobArr[i][blobHeight - j - 1] = false;
                blobArr[blobWidth - i - 1][j] = false;
                blobArr[blobWidth - i - 1][blobHeight - j - 1] = false;
            }
        }
    }
}
void Blobber::makeCircleOfSize(int size) {

}

const bool** Blobber::getBlob() {
    return (const bool**)blobArr;
}
unsigned short Blobber::getWidth() {
    return blobWidth;
}
unsigned short Blobber::getHeight() {
    return blobHeight;
}
