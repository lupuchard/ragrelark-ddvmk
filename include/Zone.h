#ifndef ZONE_H
#define ZONE_H

#include "Location.h"
#include "Tile.h"
#include "StatHolder.h"

#define MAX_ZONE_SIZE 4096
#define MAX_ZONE_WIDHEI 256

enum GenType{GEN_NONE = 0, GEN_MYDUN = 1, GEN_BSP = 2, GEN_DUNGEON = 10};

//takes up 16 bytes when not filled

class Zone: public StatHolder {
    public:
        Zone(string name, int width, int height, int lightness, bool fill);
        virtual ~Zone();
        int getWidth();
        int getHeight();
        unsigned char getLightness();
        string getName();
        Tile* getTileAt(int x, int y);
        Tile* safeGetTileAt(int x, int y);
        Location* getLocationAt(int x, int y);
        Location* getLocationAt(int i);
        Location* safeGetLocationAt(int x, int y);

        int addTile(Tile* t);
        //void setTileset(vector<Tile*>* tileSet);
        int getNumTiles();

        void fillTiles(int* tiles);
        void fillHeights(int* heights);
        void fillStructs(int* structs);

        void fill();
        bool isFilled();

        void tagDungeon(int index, int depth);
        pair<int, int> dungeonTag();
    protected:
    private:
        Location* locs;
        vector<Tile*>* tiles;
        bool ownedTiles;
        bool filled;
        unsigned char lightness; //0 - 11, 0 = blind, 1 - 10 = normal, 11 = surface
        unsigned short width;
        unsigned short height;
        string name;

        char stackIndex;
        unsigned char stackDepth;
};

#endif // ZONE_H
