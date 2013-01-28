#ifndef DATALOADER_H_INCLUDED
#define DATALOADER_H_INCLUDED

#define BETWEEN 0
#define MTG 100
#define TILE 200
#define WORLD 300
#define PLAYER 400
#define FILES 500
#define MAP 600
#define MONSTER 700
#define AREA 800
#define RESOURCES 900
#define STAIRS 1000
#define MAPSPACE 1100
#define ITEM 1200
#define STATS 1300
#define SKILLS 1350
#define UNIT 1400
#define MAPSTACK 1500
#define CONDITIONS 1650
#define MOBSPAW 1700
#define MOBEQUIPS 1800
#define ITEMSPAW 1850
#define TILEDMAPS 1900
#define TILEDMAPSREFER 1950

#define FIRST_FILE "data/base.rag"

#define DEBUG false
#define ERRCHECK true

int getStructIntFromChar(int c);
int stringToStatus(string line);

#endif // DATALOADER_H_INCLUDED
