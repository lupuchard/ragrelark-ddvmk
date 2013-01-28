#include <iostream>
#include "Tile.h"
using namespace std;

Tile::Tile(int loc, int tex, int type, int border): StatHolder(V_TILE) {
    g.loc = loc;
    g.tex = tex;
    g.type = type;
    g.border = border;

    blockMove = false;
    blockLight = false;

    over = NULL;
}

graphic Tile::getGraphic() {
    return g;
}

void Tile::setOver(Tile* t) {
    over = t;
}

Tile* Tile::getOver() {
    return over;
}

void Tile::setBlock(bool blockM, bool blockL) {
    blockLight = blockL;
    blockMove = blockM;
}

bool Tile::blocksLight() {
    return blockLight;
}

bool Tile::blocksMove() {
    return blockMove;
}
