#include <string>
#include "StatHolder.h"

#ifndef TILE_H
#define TILE_H

typedef struct {
    unsigned char loc;
    unsigned char tex;
    unsigned char type;
    unsigned char border;
} graphic;

class Tile : public StatHolder {
    public:
        //Tile();
        Tile(int loc, int tex, int type, int border);
        virtual ~Tile();
        graphic getGraphic();
    protected:
    private:
        graphic g;
};

#endif // TILE_H
