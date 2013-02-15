#ifndef RAGDRAWER_H
#define RAGDRAWER_H

#include "Texture.h"
#include "Player.h"

#define Z_MENU 4000
#define Z_FOREGROUND 3000
#define Z_EFFECT 2200
#define Z_UNIT 70
#define Z_ITEM 40
#define Z_STRUCT 30
#define Z_SPLAT 20
#define Z_GROUND 10

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} color;

const char xDirs[10] = {0, -1,  0,  1, -1,  0,  1, -1,  0,  1};
const char yDirs[10] = {0,  1,  1,  1,  0,  0,  0, -1, -1, -1};

class RagDrawer {
    public:
        RagDrawer();
        RagDrawer(int tileSize, Player* player);
        virtual ~RagDrawer();

        void drawTile(int x, int y, int z, Texture* tex, int loc);
        void drawTileRot(int x, int y, int z, Texture* tex, int loc, int rot, bool flip);
        void drawTileSpe(int x, int y, int z, Texture* tex, int x1, int y1, int size);
        void drawTileSuperSpe(int x, int y, int z, int wid, int hei, Texture* tex, int x1, int y1, int wid1, int hei1);
        void drawTileFull(int x, int y, int z, int wid, int hei, Texture* tex, int tx, int ty, int rot, bool flip);

        void drawUnit(int x, int y, Unit* unit);
        void drawColorBox(int x1, int y1, int z, int x2, int y2, color c);

        Player* getPlayer();
        int getTileSize();

        double camX;
        double camY;
    protected:
    private:
        int tileSize;
        Player* player;
};

#endif // RAGDRAWER_H
