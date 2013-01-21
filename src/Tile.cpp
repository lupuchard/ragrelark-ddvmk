#include <iostream>
#include "Tile.h"
using namespace std;

Tile::Tile(int loc, int tex, int type, int border): StatHolder(V_TILE) {
    g.loc = loc;
    g.tex = tex;
    g.type = type;
    g.border = border;
}

graphic Tile::getGraphic() {
    return g;
}
