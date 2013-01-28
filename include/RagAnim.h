#ifndef RAGANIM_H
#define RAGANIM_H

#include "RagDrawer.h"

enum animType{ANIM_NONE, ANIM_UNIT, ANIM_MOVEDIR, ANIM_MOVELOC, ANIM_ATTACK};

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

typedef struct REND_ {
    animation* anim;
    int z;
    REND_(animation* a, int pz): anim(a), z(pz) { }
    friend bool operator<(const REND_ &a, const REND_ &b) {
        if (a.z == b.z) {
            return a.anim < b.anim;
        }
        return a.z < b.z;
    }
} renderable;

class RagAnim {
    public:
        RagAnim();
        RagAnim(RagDrawer* ragDrawer);
        virtual ~RagAnim();

        void addAnim(animation* anim);
        void drawAnim(animation* anim, int z);
        void unitAnimTest(Unit* u, int x, int y);
        void renderAnims();
        void updateAnims();

        void rMoveDir(Unit* unit, int dir, int x, int y);
        void rMoveLoc(Unit* unit, int x, int y, int endX, int endY);
        void rAttack(int x, int y, int dir, int dType, int hType);
    protected:
    private:
        RagDrawer* ragd;
        int tileSize;
        vector<animation*> anims;
        set<renderable> renderables;
};

#endif // RAGANIM_H
