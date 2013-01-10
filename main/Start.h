#include <fstream>
#include <iomanip>
#include <stack>
#include <set>
#include <deque>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/glx.h>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include "fov/fov.h"
#include "Player.h"
#include "World.h"
#include "Texture.h"
#include "FormulaUser.h"
#include "usefulParsing.h"
#include "BagFolder.h"
#include "GroundFolder.h"
#include "EquipmentFolder.h"
#include "PrimeFolder.h"
#include "RandItemType.h"
#include "graphics.h"
#include "Blobber.h"

#define BOOST_IOSTREAMS_NO_LIB

#define NUM_UNIT_STATS 5

#define MA_EXAMINE 0
#define MA_GRAB 1
#define MA_DROP 2
#define MA_EQUIP 3
#define MA_EAT 4
#define MA_READ 5
#define NUM_MENU_ACTIONS 6

#define A_UNIT -1
#define A_NONE 0
#define A_MOVEDIR 1
#define A_MOVELOC 2
#define A_ATTACK 3

#define GA_NONE 0
#define GA_MOVE 1

#define ST_CONF 20
#define ST_POIS 21
#define ST_ENCUM 22

//2000-3000 calories per day
//apple = 91 calories
//763-291-6339

#define MAX_MESSAGES 100

#define INTERVAL_0_TIM 5
#define INTERVAL_1_TIM 50
#define INTERVAL_2_TIM 500
#define INTERVAL_3_TIM 5000

enum{STATE_PLAY, STATE_MENU, STATE_DIR, STATE_TARGET};
enum{SA_NONE, SA_ATTACK, SA_FIRE, SA_OPENDOOR, SA_CLOSEDOOR};

enum UnitAI{AI_STILL = 0, AI_HOSTILE = 1, AI_HOSTILESMART = 2, AI_PASSIVE = 3, AI_NEUTRAL = 4};

using namespace std;

typedef pair<string, StatHolder*> mob;

#ifndef START_H
#define START_H

class Start: FormulaUser, EnvironmentManager {
    public:
        Start();
        virtual ~Start();

        void prepare();
        void execute();

        void addMessage(string message, color c);

        /* --initiator.cpp-- */
        bool init();
        /* --- */

        /* --logic.cpp-- */
        void logic();
        /* --- */

        /* --events.cpp-- */
        void events();
        void directionPress(int dir);
        void action(int action, Unit* unit, int value1, int value2, int value3);
        void openInventory();
        void openBag();
        void openEquipment();
        void openGround();
        void groundGrab();
        bool equipItem(Item item);
        void enterCommand();
        void backCommand();
        void closeDoors();
        void itemRemovalCheck();
        void enterTargetMode();
        /* --- */

        /* --pather.cpp-- */
        void makePath(Unit* unit, short xDest, short yDest, Zone* zone, PathType special);
        void initFieldOfView();
        void playerFieldOfView(bool isNew);
        void myFovCircle(Zone* zone, void* source, int x, int y, int radius);
        void myFovCirclePerm(Zone* zone, int x, int y, int radius, int mod);
        void cleanFov();
        /* --- */

        /* --renderer.cpp-- */
        void render();
        void renderGround();
        void renderMenu();
        void renderSidePanels();
        void drawMenuBox(int x1, int y1, int x2, int y2);
        void renderMessages();
        void renderBars();
        void renderText(string text, int size, int x, int y, int z, int align, color c);
        void drawUnit(int x, int y, Unit* unit);
        void startRenderer();
        void addAnim(animation* anim);
        void makeSplatter(Unit* unit, int x, int y);
        void addStatus(string name, color c, int type);
        void removeStatus(int type);

        void drawTile(int x, int y, int z, Texture* tex, int loc);
        void drawTileRot(int x, int y, int z, Texture* tex, int loc, int rot, bool flip);
        void drawTileSpe(int x, int y, int z, Texture* tex, int x1, int y1, int size);
        void drawTileSuperSpe(int x, int y, int z, int wid, int hei, Texture* tex, int x1, int y1, int wid1, int hei1);
        void drawTileFull(int x, int y, int z, int wid, int hei, Texture* tex, int tx, int ty, int rot, bool flip);
        /* --- */

        /* --animation.cpp-- */
        void updateAnims();
        void drawAnim(animation* anim, int z);
        void unitAnimTest(Unit* u, int x, int y);
        void renderAnims();
        void rMoveDir(Unit* unit, int dir, int x, int y);
        void rMoveLoc(Unit* unit, int x, int y, int endX, int endY);
        void rAttack(int x, int y, int dir, int dType, int hType);
        /* --- */

        /* --particles.cpp-- */
        void createEffect(peType type, int x, int y);
        void updateEffects(int xShift, int yShift);
        void addProj(int x0, int y0, int x1, int y1, int length, int ind);
        void drawCirc(int x, int y, int z, int size, int fade, int rot, color c);
        void drawBox(int x, int y, int z, int size, int rote, color c);
        /* --- */

        /* --unitHandler.cpp-- */
        void ai(Unit* unit, Zone* zone);
        void moveUnit(Unit* unit, Zone* zone, int dir);
        void playerWalkStaminaDrain(int* movSpeed, int time, Unit* unit);
        void hitCMod(Unit* unit, float& damage, color& c, int& hitType, string& verb);
        void strikeUnit(Unit* unit, Zone* zone, int dir, bool safe);
        void shootUnit(Unit* attacker, Unit* defender, Zone* zone);
        void killUnit(Unit* unit, Zone* zone);
        void followPath(Unit* unit, Zone* zone);
        void reactToAttack(Unit* unit, Unit* attacker, Zone* zone);
        void goTheStairs(Unit* unit, Zone* zone);
        void openDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void closeDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void pushRock(Unit* unit, Zone* zone, int dir);
        void eatFood(Unit* unit, ItemType* food);
        void changeLoc(Unit* unit, Zone* zone, int x, int y);
        void changeLocZ(Unit* unit, Zone* prevZone, Zone* newZone, int x, int y);
        void applyPoison(int poison, int duration, Unit* unit);
        /* --- */

        /* --generator.cpp-- */
        void generateZone(Zone* zone, int tilesetType, GenType zoneType, int x1, int y1, int x2, int y2);
        void overgrowth(Zone* zone, GenType zoneType, int sx, int sy, int ex, int ey);
        /* --- */

        color dark(color c);
        color light(color c);

        ItemFolder* getItemFolder(Item item);
        void createItemFolder(Item* item);
        void putItemFolder(Item* item, ItemFolder* folder);

        void addItemToPlace(int x, int y, Zone* z, Item item);
        Item removeItemFromPlace(int x, int y, Zone* z, int index);

        /* --dataLoader.cpp-- */
        void loadData(World* w, Player* p);
        void openFile(string fileName, World* w, Player* p);
        Zone* loadTileFile(string fileName);
        void finishDataSetup();
        void deleteData();
        bool errorChecker(string filename);
        void printFileErr(string said, int line);
        void printFileProb(string said, int line);
        /* --- */

        /* --resourceLoader.cpp-- */
        void loadImage(string filename);
        void buildFont();
        Texture* getTexture(int i);
        /* --- */

        /* --formulas.cpp-- */
        int getVarValue(VOwner target, VType type, int index, StatHolderIntef* sh);
        float getVarValueF(VOwner target, VType type, int index, StatHolderIntef* sh);
        void statChanged(int stat, StatHolderIntef* statHolder);
        void conditionChanged(int condition, StatHolderIntef* statHolder);
        int getTime();
        StatHolder* findStatHolder(int target, StatHolder* statHolder);
        void parseFormula(string line, bool errCheck, int lineNum);
        /* --- */

        /* --mobSpawner.cpp */
        int addMob(string s, string tag, StatHolder* u);
        pair<string, StatHolder*> getMob(string tag);
        bool placeMob(Unit* unit, Zone* z, int x, int y, bool allowAlt, int anim);
        bool spawnMobSpeTag(int mobI, Zone* z, int x, int y, bool allowAlt, int anim);
        bool spawnMobSpe(mob m, Zone* z, int x, int y, bool allowAlt, int anim);
        int hashMob(string tag);
        int addEnvironment(string name);
        void addEncounters(int type, int level, vector<pair<pair<string, StatHolder*>, unsigned int> >* encounters);
        void createEncounters(Zone* z, int howMany, vector<pair<int, int> > possibleLocs);
        void addItemsToEncounterLevel(int type, int level, string itemSetName);
        void cleanSpawnData();
        int addItemSpawnSet(string name);
        void createItems(Zone* z, int howMany, vector<pair<int, int> > possibleLocs);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, int itemSpawnSet);
        void addItemToSpawnSet(unsigned short item, unsigned int weight, unsigned char stackMin, unsigned char stackMax, int itemSpawnSet);
        /* --- */

        /* --cleaner.cpp-- */
        void cleanup();
        /* --- */

        static const char xDirs[10];
        static const char yDirs[10];
    protected:
    private:
        SDL_Surface* display;
        SDL_Event event;
        Player* player;
        World* world;
        Area* activeArea;

        bool done;
        unsigned char state;
        unsigned char selected;
        unsigned char menuAction;
        stack<ItemFolder*> folderStack;
        PrimeFolder* primeFolder;
        map<unsigned short, vector<ItemFolder*> > folders;

        /*target & dir*/
        vector<Unit*> unitsInRange;
        short stIndex;
        unsigned short stateAction;
        /*endt*/

        map<string, Tile*> tiles;
        map<string, map<char, string>*> tileGroups;
        map<string, Area*> areas;
        map<string, vector<Zone*>*> areaZones;
        map<string, Zone*> zones;

        vector<Item> itemsToEquip;
        map<string, int> itemTypeMap;

        map<string, int> statMap;
        map<string, int> conditionMap;
        vector<pair<StatHolder*, string> > spawnPrototypes;
        vector<int> defaultStats;
        vector<Unit*> unitDeleteList;

        vector<Texture*> textures;
        Texture* structureTex;
        Texture* menuTex;
        Texture* fontTex;
        Texture* splatterTex;
        Texture* attackAnimsTex;
        Texture* playerTex;
        bool gotsStructureTex; //TODO arrayify (with enum)
        bool gotsMenuTex;
        bool gotsFontTex;
        bool gotsSplatterTex;
        bool gotsAttackAnimsTex;
        bool gotsPlayerTex;

        char tempChar;
        unsigned char loadStatus;

        bool shiftIsDown;
        vector<pair<string, color> > messages;

        short tempValues[8];
        string tempStr;
        string tempStr2;
        map<char, string>* tempMap;
        Zone* tempZone;
        Zone* tempZone2;
        Area* tempArea;
        vector<string> tempVect;
        vector<Zone*> zoneVect;

        vector<Formula*> formulas;

        int base;

        vector<animation*> anims;
        unsigned char splatters[MAX_ZONE_SIZE];
        unsigned char visibilities[MAX_ZONE_SIZE]; //0 = nope, 1 = LOS, 2 = lit

        int interval0;//TODO an array
        int interval1;
        int interval2;
        int interval3;

        map<unsigned int, Tile*> tiledTiles;
        map<unsigned int, RandItemType*> tiledItems;
        map<unsigned int, int> tiledMobs;

        set<pair<Unit*, Zone*> > areaUnits; //the zone is the zone the unit is in the zone is where the unit is
        void findAreaUnits();

        double camX;
        double camY;
        long frameTime;
};

#endif // START_H
