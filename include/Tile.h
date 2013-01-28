#include <string>
#include "StatHolder.h"

#ifndef TILE_H
#define TILE_H

enum TileType{TT_NORMAL, TT_SMOOTH, TT_SMOOTHUP, TT_SMOOTHDOWN, TT_LONGHORZ = 11, TT_OVER = 20};

typedef struct {
    unsigned char loc;
    unsigned char tex;
    unsigned char type;
    unsigned char border;
} graphic;

class Tile : public StatHolder {
    public:
        Tile(int loc, int tex, int type, int border);
        void setOver(Tile* t);
        void setBlock(bool blockM, bool blockL);

        graphic getGraphic();
        Tile* getOver();
        bool blocksLight();
        bool blocksMove();
    protected:
    private:
        graphic g;
        Tile* over;
        bool blockLight;
        bool blockMove;
};

#endif // TILE_H
