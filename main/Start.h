/*
 *  Copyright 2013 Luke Puchner-Hardman
 *
 *  This file is part of Ragrelark.
 *  Ragrelark is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Ragrelark is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Ragrelark.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef START_H
#define START_H

#include "main.h"
#include "fov/fov.h"

#include "Player.h"
#include "Swarmer.h"
#include "World.h"
#include "PrimeFolder.h"
#include "graphics.h"
#include "Ability.h"
#include "TiledLoader.h"

#define BOOST_IOSTREAMS_NO_LIB

enum Status{ST_NONE, ST_CONF, ST_POIS, ST_ENCUM, ST_HUNG, ST_DEAD};

#define MAX_MESSAGES 100
#define NUM_NOTELINES 26

#define INTERVAL_0_TIM 5
#define INTERVAL_1_TIM 50
#define INTERVAL_2_TIM 500
#define INTERVAL_3_TIM 5000

enum MenuAction{MA_GRAB, MA_DROP, MA_EQUIP, MA_CONSUME, MA_THROW, NUM_MENU_ACTIONS};
enum{STATE_PLAY, STATE_DEAD, STATE_MENU, STATE_DIR, STATE_TARGET, STATE_SPELL};
enum Panels{PANEL_EMPTY, PANEL_TOPSTART, PANEL_STATS, PANEL_SKILLS, PANEL_INVENTORY, PANEL_TOPEND, PANEL_BOTTOMSTART, PANEL_MINIMAP, PANEL_NOTES, PANEL_BOTTOMEND};
enum UnitAI{AI_STILL = 0, AI_HOSTILE = 1, AI_HOSTILESMART = 2, AI_PASSIVE = 3, AI_NEUTRAL = 4};

enum StateAction{SA_NONE, SA_ATTACK, SA_FIRE, SA_THROW, SA_OPENDOOR, SA_CLOSEDOOR};

enum FlavorClass{F_NONE, F_POISON_PHYS, F_POISON_MENT, F_POISON_REGEN, F_POISON_EXTRA, F_CONFUSION};
struct Flavor {
    std::map<FlavorClass, int> flavors;
    void add(FlavorClass f, int potency = 1) {
        flavors[f] = potency;
    }
};

struct BattleSummary {
    int criticality;
    bool killed;
    bool hit;
    bool dodge;
};

class Start: FormulaUser, EnvironmentManager {
    public:
        Start();
        virtual ~Start();

        bool prepare();
        void execute();

        void addMessage(String message, Color c);

        Skill* skll(SkillE skillIndex);

        void parsePlayer(YAML::Node n, std::ostream& lerr);

        /* --initiator.cpp-- */
        bool init();
        /* --- */

        /* --logic.cpp-- */
        void logic();
        /* --- */

        /* --events.cpp-- */
        void events();
        void directionPress(int dir);
        void sapExp(Unit* sapper, Unit* target, Skill* skill, int multitude);
        void debankExp(Unit* debanker, Skill* skill, int amount);
        void action(Skill* skill, int exp);
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
        void makePath(Unit* unit, Coord dest, Zone* zone, PathType special);
        void initFieldOfView();
        void playerFieldOfView(bool isNew);
        void myFovCircle(Zone* zone, void* source, Coord c, int radius);
        void myFovCirclePerm(Zone* zone, Coord c, int radius, int mod);
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
        void renderText(String text, int size, int x, int y, int z, int align, Color c);
        void startRenderer();
        void makeSplatter(Unit* unit, Zone* zone, Coord loc);
        void addStatus(String name, Color c, Status type);
        void removeStatus(Status type);
        /* --- */

        /* --particles.cpp-- */
        void createEffect(PeType type, int x, int y);
        void updateEffects(int xShift, int yShift);
        void addProj(int x0, int y0, int x1, int y1, int length, int ind);
        void drawCirc(int x, int y, int z, int size, int fade, int rot, Color c);
        void drawBox(int x, int y, int z, int size, int rote, Color c);
        /* --- */

        /* --unitHandler.cpp-- */
        void ai(Unit* unit, Zone* zone);
        void followPath(Unit* unit, Zone* zone);
        void playerWalkStaminaDrain(int* movSpeed, int time, Unit* unit);
        void moveUnit(Unit* unit, Zone* zone, int dir);

        void goTheStairs(Unit* unit, Zone* zone);
        void changeLoc(Unit* unit, Zone* zone, Coord loc);
        void changeLocZ(Unit* unit, Zone* prevZone, Zone* newZone, Coord loc);

        int actionTimePassed(int time, int speed);
        double chanceHit(double m);
        double defDam(double preDam, int defense);
        String defenderNoun(Unit* attacker, Unit* defender);
        Color colorHit(bool crit, bool dodge);

        void hitCMod(Unit* defender, float& damage, float accuracy, int& hitType, BattleSummary& sum);
        BattleSummary attackUnit(float power, float accuracy, const WeapType* weapType, Unit* defender, Zone* zone, int dir, Flavor flavor = Flavor());
        void pfaf(int stat, int potency, Unit* u);
        void strikeUnit(Unit* unit, Zone* zone, int dir, bool safe);
        void throwItem(Item item, Unit* unit, Coord target, Zone* zone);
        void shootUnit(Unit* attacker, Item shooter, Item beingShot, Unit* defender, Zone* zone);
        BattleSummary projectItem(Item item, int power, int accuracy, Zone* zone, Coord to, Coord from);
        void reactToAttack(Unit* unit, int dir, Zone* zone);
        void killUnit(Unit* unit, Zone* zone);

        void openDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void closeDoor(Unit* unit, Zone* zone, int dir, bool safe);
        void pushRock(Unit* unit, Zone* zone, int dir);
        void eatFood(Unit* unit, ItemType* food);
        void search(Unit* unit, Zone* zone);
        /* --- */

        /* --abilities.cpp-- */
        void castSpell(unsigned int spellI, Unit* unit, Zone* zone);
        /* --- */

        ItemFolder* getItemFolder(Item item);
        void putItemFolder(Item* item, ItemFolder* folder);

        void addItemToPlace(Coord pos, Zone* z, Item item);
        Item removeItemFromPlace(Coord pos, Zone* z, int index);

        /* --dataLoader.cpp-- */
        bool loadData();
        bool yamlSingle(String file, boost::function<void(YAML::Node, std::ostream&)> parseFunc);
        bool yamlMulti(String directory, boost::function<void(YAML::Node, std::ostream&)> parseFunc);
        void deleteData();
        bool errorChecker(String filename);
        void printFileErr(String said, int line);
        void printFileProb(String said, int line);
        /* --- */

        /* --formulas.cpp-- */
        int getVarValue(VOwner target, VType type, int index, StatHolderIntef* sh);
        float getVarValueF(VOwner target, VType type, int index, StatHolderIntef* sh);
        void statChanged(int stat, StatHolderIntef* statHolder);
        void conditionChanged(int condition, StatHolderIntef* statHolder);
        int getTime();
        StatHolder* findStatHolder(int target, StatHolder* statHolder);
        /* --- */

        /* --cleaner.cpp-- */
        void cleanup();
        /* --- */
    private:
        SDL_Surface* display;
        SDL_Event event;
        Player* player;
        World* world;
        Area* activeArea;
        std::vector<Unit*> unitDeleteList;
        MobSpawner* mobSpawner;
        std::set<std::pair<Unit*, Zone*> > areaUnits;
        void findAreaUnits();

        bool done;
        short selected;
        int state;
        int menuAction;
        std::stack<ItemFolder*> folderStack;
        PrimeFolder* primeFolder;
        std::map<unsigned short, std::vector<ItemFolder*> > folders;
        unsigned char visibilities[MAX_ZONE_SIZE]; //0 = nope, 1 = LOS, 2 = lit
        unsigned char loadStatus;

        /*target & dir*/
        std::vector<Unit*> unitsInRange;
        short stIndex;
        StateAction stateAction;
        /*endt*/
        bool circleSelect[10];
        int topPanel;
        int botPanel;
        int notesSelected;
        String theNotes[NUM_NOTELINES];

        Texture *structureTex, *menuTex, *fontTex, *splatterTex, *attackAnimsTex, *playerTex;
        TiledLoader* tiledLoader;
        std::ostringstream loadErrStream;

        RagDrawer ragd;
        RagAnim raga;

        bool shiftIsDown;
        std::vector<std::pair<String, Color> > messages;
        int intervals[4];
        long frameTime;

        int foon;

        std::vector<Formula*> formulas;
        int base;
};

#endif // START_H
