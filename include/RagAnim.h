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

#ifndef RAGANIM_H
#define RAGANIM_H

#include "RagDrawer.h"

enum AnimType{ANIM_NONE, ANIM_UNIT, ANIM_MOVEDIR, ANIM_MOVELOC, ANIM_ATTACK};

struct Animation {
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
    Animation* nextAnim;
}; //28 bytes

struct Renderable {
    Animation* anim;
    int z;
    Renderable(Animation* a, int pz): anim(a), z(pz) { }
    friend bool operator<(const Renderable &a, const Renderable &b) {
        if (a.z == b.z) {
            return a.anim < b.anim;
        }
        return a.z < b.z;
    }
};

class RagAnim {
    public:
        RagAnim();
        RagAnim(RagDrawer* ragDrawer, Texture* animTex);
        virtual ~RagAnim();

        void addAnim(Animation* anim);
        void drawAnim(Animation* anim, int z);
        void unitAnimTest(Unit* u, int x, int y);
        void renderAnims();
        void updateAnims();

        void rMoveDir(Unit* unit, int dir, Coord loc);
        void rMoveLoc(Unit* unit, Coord begin, Coord end);
        void rAttack(Coord loc, int dir, int dType, int hType);
    private:
        RagDrawer* ragd;
        int tileSize;
        std::vector<Animation*> anims;
        std::set<Renderable> renderables;
        Texture* animTex;
};

#endif // RAGANIM_H
