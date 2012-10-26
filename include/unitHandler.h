#include "Start.h"

#ifndef UNITHANDLER_H_INCLUDED
#define UNITHANDLER_H_INCLUDED

Player* hPlayer;void moveUnit(Unit* unit, Zone* zone, int dir);
void openDoor(Unit* unit, Zone* zone, int dir, bool safe);
void changeLoc(Unit* unit, Zone* zone, int x, int y);

#endif // UNITHANDLER_H_INCLUDED
