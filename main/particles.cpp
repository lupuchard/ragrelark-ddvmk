#include "Start.h"

#include <list>

union ParticleS {short c; struct {char b1; char b2;} b;};
union ParticleI {
    int e;
    float f;
    unsigned int i;
};
union ParticleL {
    double g;
    long l;
};
union ParticleLL {
    struct {long l; double d;} ld;
    struct {
        short x0;
        short y0;
        short x1;
        short y1;
        int timStart;
        short len;
        unsigned char ind;
        unsigned char rotFlip;
    } proj;
};
struct ParticleEffectS {
    ParticleS* particles;
    unsigned int numParticles;
    int startTime;
    short x;
    short y;
};
struct ParticleEffectPerm {
    list<ParticleLL> particles;
};

//TODO cleanup

multimap<peType, void*> particleEffects;

void Start::addProj(int x0, int y0, int x1, int y1, int length, int ind) {
    ParticleEffectPerm* arrow = (ParticleEffectPerm*)particleEffects.find(P_ARROW)->second;
    ParticleLL foon;
    foon.proj.x0 = x0; foon.proj.y0 = y0;
    foon.proj.x1 = x1; foon.proj.y1 = y1;
    foon.proj.len = length;
    int rotation = (int)(atan2(y1 - y0, x1 - x0) / 3.14 * 180 + 367) % 360;
    if (rotation > 270 || (rotation >= 90 && rotation < 180)) {
        rotation = (rotation + 180) % 360;
    }
    int rotX;
    bool rotF;
    int rotZ;
    if (rotation % 90 < 45) {
        rotX = (rotation % 45) / 15;
        rotF = true;
        rotZ = rotation / 90;
    } else {
        rotX = 3 - (rotation % 45) / 15;
        rotF = false;
        rotZ = 3 - ((rotation / 90) % 4);
    }
    foon.proj.ind = ind * 4 + rotX;
    foon.proj.rotFlip = rotZ;
    if (rotF) foon.proj.rotFlip += 4;
    foon.proj.timStart = frameTime;
    arrow->particles.push_back(foon);
}
void Start::createEffect(peType type, int x, int y) {
    switch (type) {
        case P_NONE: break;
        case P_DARKDUST: {
            ParticleEffectS* p = new ParticleEffectS;
            p->startTime = frameTime;
            p->numParticles = 10;
            p->x = x; p->y = y;
            p->particles = new ParticleS[p->numParticles];
            for (unsigned int i = 0; i < p->numParticles; i++) {
                ParticleS part;
                part.b.b1 = rand() % 180;
                part.b.b2 = rand() % 100;
                p->particles[i] = part;
            }
            particleEffects.insert(pair<peType, void*>(type, p));
        } break;
        case P_ARROW: {
            particleEffects.insert(pair<peType, void*>(type, new ParticleEffectPerm));
        } break;
        default: break;
    }
}


void Start::updateEffects(int xCam, int yCam) {
    for (multimap<peType, void*>::iterator it = particleEffects.begin(); it != particleEffects.end(); it++) {
        switch(it->first) {
            case P_NONE: break;
            case P_DARKDUST: {
                ParticleEffectS* explosion = (ParticleEffectS*)(it->second);
                int x = explosion->x + xCam;
                int y = explosion->y + yCam;
                int timePassed = frameTime - explosion->startTime;
                if (timePassed >= 20) {
                    delete[] explosion->particles;
                    delete explosion;
                    particleEffects.erase(it);
                    break;
                }
                for (unsigned int i = 0; i < explosion->numParticles; i++) {
                    ParticleS part = explosion->particles[i];
                    float f = (part.b.b2 + 50) / 150.f;
                    color c = black;
                    c.alpha = 255 - 12 * timePassed;
                    drawCirc(x + (int)(timePassed * cos(part.b.b1 / 3.14) * f),
                             y + (int)(timePassed * sin(part.b.b1 / 3.14) * f), Z_EFFECT + i, 2, 0, 0, c);
                }
            } break;
            case P_ARROW: {
                ParticleEffectPerm* arrows = (ParticleEffectPerm*)(it->second);
                int iv = 0;
                for (list<ParticleLL>::iterator i = arrows->particles.begin(); i != arrows->particles.end(); ) {
                    if (frameTime >= i->proj.timStart + i->proj.len) {
                        arrows->particles.erase(i++);
                    } else {
                        float timePassed = (frameTime - i->proj.timStart) / (float)i->proj.len;
                        int xChange = (int)((i->proj.x1 - i->proj.x0) * timePassed);
                        int yChange = (int)((i->proj.y1 - i->proj.y0) * timePassed);

                        int x = i->proj.x0 + xChange;
                        int y = i->proj.y0 + yChange;
                        ragd.drawTileFull(x, y, Z_EFFECT + iv, 32, 32, getAttackAnimsTex(), 32 * i->proj.ind, 992, i->proj.rotFlip % 4, i->proj.rotFlip / 4);
                        i++;
                    }
                    iv++;
                }
            } break;
            default: break;
        }
    }
}

//box, (288,544), 32x32, -4, 6
//circ, (0, 544), 3x3, 2, 16

void Start::drawCirc(int x, int y, int z, int size, int fade, int rot, color c) {
    glColor4f(c.red / 255., c.green / 255., c.blue / 255., c.alpha / 255.);
    int wid = 3 + 2 * size;
    ragd.drawTileFull(x, y, z, wid, wid, getAttackAnimsTex(), size * (size + 2), 544 + wid * fade, rot, false);
    glColor4f(1.f, 1.f, 1.f, 1.f);
}

int alignX[] = {0, 0, 1, 1};
int alignY[] = {0, 1, 1, 0};
void Start::drawBox(int x, int y, int z, int size, int rote, color c) {
    glColor4f(c.red / 255., c.green / 255., c.blue / 255., c.alpha / 255.);
    rote = rote % 90;
    int wid = 12 + 4 * size;
    int f;
    bool flip;
    if (rote < 45) {
        f = (rote % 45) / 5;
        flip = false;
    } else {
        f = 9 - (rote % 45) / 5;
        flip = true;
    }
    ragd.drawTileFull(x + flip, y, z, wid, wid, getAttackAnimsTex(), 420 - 2 * (size + 1) * (size + 6), 544 + wid * f, 0, flip);
    glColor4f(1.f, 1.f, 1.f, 1.f);
}
