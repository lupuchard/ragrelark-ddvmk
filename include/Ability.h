#ifndef ABILITY_H
#define ABILITY_H

#include <string>

#define SPELL_LIGHT 768

using namespace std;

class Ability {
    public:
        Ability(string name, int manaCost, int difficulty, int timeTake, int classI);

        string getName();
        int getCost();
        int getDifficulty();
        int getTimeTake();
        int getI();
    protected:
    private:
        string name;
        unsigned short manaCost;
        unsigned short difficulty;
        unsigned short classificationI;
        unsigned short timeTake;
};

void addAbility(Ability* ability);
Ability* getAbility(int index);
Ability* getAbility(bool wind, bool earth, bool water, bool fire, bool phys, bool ment, bool light, bool dark, int power);
void cleanAbilities();

#endif // ABILITY_H
