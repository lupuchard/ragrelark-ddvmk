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
#include "PrimeFolder.h"
#include "graphics.h"
#include "Ability.h"

using namespace std;

#define BOOST_IOSTREAMS_NO_LIB

#define ST_CONF 20
#define ST_POIS 21
#define ST_ENCUM 22
#define ST_HUNG 23

//2000-3000 calories per day
//apple = 91 calories
//763-291-6339

#define MAX_MESSAGES 100
#define NUM_NOTELINES 26

#define INTERVAL_0_TIM 5
#define INTERVAL_1_TIM 50
#define INTERVAL_2_TIM 500
#define INTERVAL_3_TIM 5000

enum MenuAction{MA_EXAMINE, MA_GRAB, MA_DROP, MA_EQUIP, MA_EAT, MA_READ, NUM_MENU_ACTIONS};
enum{STATE_PLAY, STATE_MENU, STATE_DIR, STATE_TARGET, STATE_SPELL};
enum Panels{PANEL_EMPTY, PANEL_TOPSTART, PANEL_STATS, PANEL_SKILLS, PANEL_INVENTORY, PANEL_TOPEND, PANEL_BOTTOMSTART, PANEL_MINIMAP, PANEL_NOTES, PANEL_BOTTOMEND};
enum UnitAI{AI_STILL = 0, AI_HOSTILE = 1, AI_HOSTILESMART = 2, AI_PASSIVE = 3, AI_NEUTRAL = 4};

enum AttackType{ATT_STRIKE, ATT_SHOOT, ATT_SPELL};
enum{SA_NONE, SA_ATTACK, SA_FIRE, SA_OPENDOOR, SA_CLOSEDOOR};

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
        void sapExp(Unit* sapper, Unit* target, SkillType skill, int multitude);
        void debankExp(Unit* debanker, SkillType skill, int amount);
        void action(SkillType skill, int exp);
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
        void enterSpellMode();
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
        void renderCircleSelect();
        void renderBars();
        void renderText(string text, int size, int x, int y, int z, int align, color c);
        void startRenderer();
        void makeSplatter(Unit* unit, int x, int y);
        void addStatus(string name, color c, int type);
        void removeStatus(int type);
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
        void followPath(Unit* unit, Zone* zone);
        void playerWalkStaminaDrain(int* movSpeed, int time, Unit* unit);
        void moveUnit(Unit* unit, Zone* zone, int dir);

        void goTheStairs(Unit* unit, Zone* zone);
        void changeLoc(Unit* unit, Zone* zone, int x, int y);
        void changeLocZ(Unit* unit, Zone* prevZone, Zone* newZone, int x, int y);

        void applyPoison(int poison, int duration, Unit* unit, Unit* poisoner);
        int actionTimePassed(int time, int speed);

        void hitCMod(Unit* unit, float& damage, color& c, int& hitType, string& verb, int unarmedAttackType);
        void strikeUnit(Unit* unit, Zone* zone, int dir, bool safe);
        void shootUnit(Unit* attacker, Unit* defender, Zone* zone);
        void attackUnit(Unit* attacker, Unit* defender, Zone* zone, int dir, AttackType attackType);
        void reactToAttack(Unit* unit, Unit* attacker, Zone* zone);
        void killUnit(Unit* unit, Zone* zone);

        void openDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void closeDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void pushRock(Unit* unit, Zone* zone, int dir);
        void eatFood(Unit* unit, ItemType* food);
        /* --- */

        /* --abilities.cpp-- */
        void castSpell(unsigned int spellI, Unit* unit, Zone* zone);
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
        Zone* loadTileFile(string fileName, string zoneName);
        void finishDataSetup();
        void deleteData();
        bool errorChecker(string filename);
        void printFileErr(string said, int line);
        void printFileProb(string said, int line);
        /* --- */

        /* --resourceLoader.cpp-- */
        void loadImage(string filename);
        void buildFont();
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

        /* --cleaner.cpp-- */
        void cleanup();
        /* --- */
    protected:
    private:
        SDL_Surface* display;
        SDL_Event event;
        Player* player;
        World* world;
        Area* activeArea;

        bool done;
        short selected;
        int state;
        int menuAction;
        stack<ItemFolder*> folderStack;
        PrimeFolder* primeFolder;
        map<unsigned short, vector<ItemFolder*> > folders;

        /*target & dir*/
        vector<Unit*> unitsInRange;
        short stIndex;
        unsigned short stateAction;
        /*endt*/
        bool circleSelect[10];
        int topPanel;
        int botPanel;
        int notesSelected;
        string theNotes[NUM_NOTELINES];

        bool gotsStructureTex; //TODO arrayify (with enum)
        bool gotsMenuTex;
        bool gotsFontTex;
        bool gotsSplatterTex;
        bool gotsAttackAnimsTex;
        bool gotsPlayerTex;

        RagDrawer ragd;
        RagAnim raga;

        unsigned char loadStatus;

        bool shiftIsDown;
        vector<pair<string, color> > messages;

        /* temp data for loading */
        short tempValues[16];
        char tempChar;
        string tempStr;
        string tempStr2;
        map<char, string>* tempMap;
        Zone* tempZone;
        Zone* tempZone2;
        Area* tempArea;
        DungeonStack* tempDun;
        vector<string> tempVect;
        vector<Zone*> zoneVect;

        map<string, vector<Zone*>*> areaZones;
        map<string, map<char, string>*> tileGroups;
        map<string, int> itemTypeMap;
        map<string, int> statMap;
        map<string, int> conditionMap;
        map<string, int> spellMap;
        map<string, int> skillMap;
        map<string, MobEquipSet*> mobEquipsMap;
        vector<Item> itemsToEquip;
        /* end temp values for loading */

        vector<pair<StatHolder*, string> > spawnPrototypes;
        vector<int> defaultStats;
        vector<Unit*> unitDeleteList;

        map<string, Tile*> tiles;
        map<string, Area*> areas;
        map<string, Zone*> zones;
        map<string, DungeonStack*> dungeons;

        vector<Formula*> formulas;
        int base;

        unsigned char splatters[MAX_ZONE_SIZE];
        unsigned char visibilities[MAX_ZONE_SIZE]; //0 = nope, 1 = LOS, 2 = lit

        int interval0;//TODO an array
        int interval1;
        int interval2;
        int interval3;
        int foon;

        map<unsigned int, Tile*> tiledTiles;
        map<unsigned int, RandItemType*> tiledItems;
        map<unsigned int, int> tiledMobs;

        set<pair<Unit*, Zone*> > areaUnits; //the zone is the zone the unit is in the zone is where the unit is
        void findAreaUnits();

        long frameTime;

        MobSpawner* mobSpawner;
};

#endif // START_H
