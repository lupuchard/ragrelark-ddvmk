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
#include <fov/fov.h>
#include "Player.h"
#include "World.h"
#include "Texture.h"
#include "FormulaUser.h"
#include "usefulParsing.h"
#include "BagFolder.h"
#include "GroundFolder.h"
#include "EquipmentFolder.h"
#include "PrimeFolder.h"

#define NUM_UNIT_STATS 5

#define WIN1_HEIGHT 600
#define WIN1_WIDTH 640
#define CWIN_HEIGHT 100
#define CWIN_WIDTH 640

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

#define G_NONE 0
#define G_MYDUN 1
#define G_BSP 2
#define G_DUNGEON 10
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

using namespace std;
//using namespace boost::algorithm;

typedef struct ANMAT_ {
    short type;
    unsigned char value;
    unsigned char dir;
    unsigned short time;
    unsigned short end;
    short startX;
    short startY;
    int ZY;
    float temp;
    Unit* target;
    ANMAT_* nextAnim;
} animation; //28 bytes

typedef struct {
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
} color;

const color red        = {255,   0,   0, 0};  const color scarlet    = {255,  63,   0, 0};  const color orange     = {255, 127,   0, 0};
const color amber      = {255, 191,   0, 0};  const color yellow     = {255, 255,   0, 0};  const color lime       = {191, 255,   0, 0};
const color chartreuse = {127, 255,   0, 0};  const color harlequin  = { 63, 255,   0, 0};  const color green      = {  0, 255,   0, 0};
const color jade       = {  0, 255,  63, 0};  const color spring     = {  0, 255, 127, 0};  const color turquoise  = {  0, 255, 191, 0};
const color cyan       = {  0, 255, 255, 0};  const color aqua       = {  0, 191, 255, 0};  const color azure      = {  0, 127, 255, 0};
const color cobalt     = {  0,  63, 255, 0};  const color blue       = {  0,   0, 255, 0};  const color iris       = { 63,   0, 255, 0};
const color violet     = {127,   0, 255, 0};  const color amethyst   = {191,   0, 255, 0};  const color magenta    = {255,   0, 255, 0};
const color fuchsia    = {255,   0, 191, 0};  const color rose       = {255,   0, 127, 0};  const color crimson    = {255,   0,  63, 0};

const color maroon     = {127,   0,   0, 0};  const color brick      = {191,   0,   0, 0};  const color pink       = {255, 127, 127, 0};  const color salmon     = {191,  63,  63, 0};
const color olive      = {127, 127,   0, 0};  const color cream      = {255, 255, 127, 0};  const color olivine    = {191, 191,  63, 0};
const color forest     = {  0, 127,   0, 0};  const color grass      = {127, 255, 127, 0};  const color fern       = { 63, 191,  63, 0};
const color teal       = {  0, 127, 127, 0};  const color celeste    = {127, 255, 255, 0};  const color aquamarine = { 63, 191, 191, 0};
const color navy       = {  0,   0, 127, 0};  const color sky        = {127, 127, 255, 0};  const color glaucous   = { 63,  63, 191, 0};
const color purple     = {127,   0, 127, 0};  const color thistle    = {255, 127, 255, 0};  const color lavender   = {191,  63, 191, 0};

const color brown      = {127,  63,   0, 0};  const color swamp      = {  0, 127,  63, 0};  const color indigo     = { 63,   0, 127, 0};  const color tann       = {223, 191, 127, 0};

const color white      = {255, 255, 255, 0};  const color shine      = {223, 223, 223, 0};  const color silver     = {191, 191, 191, 0};  const color gray       = {127, 127, 127, 0};
const color grey       = { 63,  63,  63, 0};  const color charcoal   = { 31,  31,  31, 0};  const color tar        = { 15,  15,  15, 0};  const color black      = {  0,   0,   0, 255};

typedef pair<string, StatHolder*> mob;

#ifndef START_H
#define START_H

class Start: FormulaUser, EnvironmentManager {
    public:
        Start();
        virtual ~Start();

        void prepare();
        void execute();
        bool init();
        void logic();

        void events();
        void directionPress(int dir);
        void action(int action, Unit* unit, int value1, int value2, int value3);

        void makePath(Unit* unit, short xDest, short yDest, Zone* zone, int special);

        void initFieldOfView();
        void playerFieldOfView(bool isNew);
        void myFovCircle(Zone* zone, void* source, int x, int y, int radius);
        void myFovCirclePerm(Zone* zone, int x, int y, int radius, int mod);

        void render();
        void renderGround();
        void renderMenu();
        void drawMenuBox(int x1, int y1, int x2, int y2);
        void renderMessages();
        void renderBars();
        void renderText(string text, int size, int x, int y, int z, int align, color c);
        void drawUnit(int x, int y, Unit* unit);
        void drawAnim(animation* anim, int z);
        void startRenderer();
        void rMoveDir(Unit* unit, int dir, int x, int y);
        void rMoveLoc(Unit* unit, int x, int y, int endX, int endY);
        void rAttack(int x, int y, int dir, int dType, int hType);
        void addAnim(animation* anim);
        void makeSplatter(Unit* unit, int x, int y);
        void addStatus(string name, color c, int type);
        void removeStatus(int type);

        void ai(Unit* unit, Zone* zone);
        void moveUnit(Unit* unit, Zone* zone, int dir);
        void playerWalkStaminaDrain(int* movSpeed, int time, Unit* unit);
        void attackUnit(Unit* unit, Zone* zone, int dir, bool safe);
        void killUnit(Unit* unit, Zone* zone);
        void goTheStairs(Unit* unit, Zone* zone);
        void openDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void closeDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void pushRock(Unit* unit, Zone* zone, int dir);
        void eatFood(Unit* unit, ItemType* food);
        void changeLoc(Unit* unit, Zone* zone, int x, int y);
        void changeLocZ(Unit* unit, Zone* prevZone, Zone* newZone, int x, int y);
        void applyPoison(int poison, int duration, Unit* unit);

        void addItemToPlace(int x, int y, Zone* z, Item item);
        Item removeItemFromPlace(int x, int y, Zone* z, int index);

        //Unit* createUnit(StatHolder* prototype, string name, int x, int y, Zone* zone);

        void generateZone(Zone* zone, int tilesetType, int zoneType, int x1, int y1, int x2, int y2);

        color dark(color c);
        color light(color c);

        void openInventory();
        void openBag();
        void openEquipment();
        void openGround();
        void groundGrab();
        bool equipItem(Item item);
        void enterCommand();
        void backCommand();
        void closeDoors();

        ItemFolder* getItemFolder(Item item);
        void createItemFolder(Item* item);
        void putItemFolder(Item* item, ItemFolder* folder);

        void loadData(World* w, Player* p);
        void loadImage(string filename);
        void buildFont();
        void finishDataSetup();
        void deleteData();
        Texture* getTexture(int i);

        void cleanup();
        void cleanFov();

        void addMessage(string message, color c);

        int getVarValue(VOwner target, VType type, int index, StatHolderIntef* sh);
        float getVarValueF(VOwner target, VType type, int index, StatHolderIntef* sh);
        void statChanged(int stat, StatHolderIntef* statHolder);
        void conditionChanged(int condition, StatHolderIntef* statHolder);
        int getTime();
        StatHolder* findStatHolder(int target, StatHolder* statHolder);

        int addMob(string s, string tag, StatHolder* u);
        //pair<string, StatHolder*> getMob(int i);
        pair<string, StatHolder*> getMob(string tag);
        bool placeMob(Unit* unit, Zone* z, int x, int y, bool allowAlt, int anim);
        bool spawnMobSpeTag(int mobI, Zone* z, int x, int y, bool allowAlt, int anim);
        bool spawnMobSpe(mob m, Zone* z, int x, int y, bool allowAlt, int anim);
        int hashMob(string tag);
        //int findMob(string s);
        int addEnvironment(string name);
        void addEncounters(int type, int level, vector<pair<pair<string, StatHolder*>, unsigned int> >* encounters);
        void createEncounters(Zone* z, int howMany, vector<pair<int, int> > possibleLocs);
        void addItemsToEncounterLevel(int type, int level, string itemSetName);

        void cleanSpawnData();
        int addItemSpawnSet(string name);
        void createItems(Zone* z, int howMany, vector<pair<int, int> > possibleLocs);
        void addItemToSpawnSet(unsigned short item, unsigned short weight, int itemSpawnSet);

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
        unsigned char menuUp;
        unsigned char selected;
        unsigned char menuAction;
        stack<ItemFolder*> folderStack;
        PrimeFolder* primeFolder;
        map<unsigned short, vector<ItemFolder*> > folders;

        void openFile(string fileName, World* w, Player* p);
        map<string, Tile*> tiles;
        map<string, map<char, string>*> tileGroups;
        map<string, Area*> areas;
        map<string, vector<Zone*>*> areaZones;
        map<string, Zone*> zones;

        vector<Item> itemsToEquip;
        map<string, int> itemTypeMap;

        map<string, int> statMap;
        map<string, int> conditionMap;
        //map<string, int> mobMap;
        vector<pair<StatHolder*, string> > spawnPrototypes;
        vector<int> defaultStats;
        vector<Unit*> unitDeleteList;

        vector<Texture*> textures;
        Texture* structureTex;
        Texture* menuTex;
        Texture* fontTex;
        Texture* splatterTex;
        Texture* attackAnimsTex;
        bool gotsStructureTex;
        bool gotsMenuTex;
        bool gotsFontTex;
        bool gotsSplatterTex;
        bool gotsAttackAnimsTex;

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

        void itemRemovalCheck();

        int interval0;
        int interval1;
        int interval2;
        int interval3;

        set<pair<Unit*, Zone*> > areaUnits; //the zone is the zone the unit is in the zone is where the unit is
        void findAreaUnits();
};

#endif // START_H
