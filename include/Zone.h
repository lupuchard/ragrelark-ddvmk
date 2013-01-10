#ifndef ZONE_H
#define ZONE_H

#include "Location.h"
#include "Tile.h"
#include "StatHolder.h"

#define MAX_ZONE_SIZE 4096
#define MAX_ZONE_WIDHEI 256

enum GenType{GEN_NONE = 0, GEN_MYDUN = 1, GEN_BSP = 2, GEN_DUNGEON = 10};

//takes up 16 bytes when not filled

typedef struct {
    short x;
    short y;
    short value;
    char type;
} modification;

class Zone: public StatHolder {
    public:
        Zone(int width, int height, int numDiffTiles, int lightness, int fill, int mType);
        virtual ~Zone();
        int getWidth();
        int getHeight();
        unsigned char getLightness();
        Tile* getTileAt(int x, int y);
        Tile* safeGetTileAt(int x, int y);
        Location* getLocationAt(int x, int y);
        Location* getLocationAt(int i);
        Location* safeGetLocationAt(int x, int y);

        int addTile(Tile* t);
        void setTileset(int num, Tile** tileset);
        int getNumTiles();

        void fillTiles(int* tiles);
        void fillHeights(int* heights);
        void fillStructs(int* structs);

        void becomeGenned();
        void fill();
        int isFilled();
        void addModification(int x, int y, char type, int value);

        GenType getGenType();
        int getMobType();
        unsigned char getMobSpawnLevel();
        void setMobSpawnLevel(unsigned char mobSpawnLevel);

        int getNumReserved();
        pair<int, int> getRes(int index);
    protected:
    private:
        Location* locs;
        Tile** tiles;
        char numTiles; //negative means shared tileset
        char filled; //0 = unfilled, 1 = filled & generated, negative is unfilled and specifies generation type
        unsigned char lightness; //0 - 11, 0 = blind, 1 - 10 = normal, 11 = surface
        unsigned char mobType;
        unsigned char mobSpawnLevel;
        unsigned short width;
        unsigned short height;
        vector<modification>* modifications;
        bool hasModifications;
};

#endif // ZONE_H
