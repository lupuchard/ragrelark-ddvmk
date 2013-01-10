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
            if (dist <= rad) {
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
