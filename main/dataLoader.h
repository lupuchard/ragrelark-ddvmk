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
#define CONNECTION 1000
#define MAPSPACE 1100
#define ITEM 1200
#define STATS 1300
#define UNIT 1400
#define MAPSTACK 1500
#define FEATURE 1600
#define CONDITIONS 1700
#define MOBSPAW 1800
#define ITEMSPAW 1900
#define TILEDMAPS 2000
#define TILEDMAPSREFER 2100

#define FIRST_FILE "data/base.txt"

#define DEBUG false
#define ERRCHECK true

int getStructIntFromChar(int c);
int stringToStatus(string line);

#endif // DATALOADER_H_INCLUDED
