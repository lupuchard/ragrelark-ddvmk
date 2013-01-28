#ifndef CTEXTURE_H
#define CTEXTURE_H

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>
#include <GL/gl.h>
#include <vector>

using namespace std;

class Texture {
    public:
        Texture();

        bool load(string file);
        bool load(SDL_Surface* surface);
        void bind();
        void cleanup();

        int         width;
        int         height;
        GLuint     textureID;
};

int addTexture(Texture* texture);
void setStructureTex(Texture* texture);
void setMenuTex(Texture* texture);
void setFontTex(Texture* texture);
void setSplatterTex(Texture* texture);
void setAttackAnimsTex(Texture* texture);
void setPlayerTex(Texture* texture);
Texture* getStructureTex();
Texture* getMenuTex();
Texture* getFontTex();
Texture* getSplatterTex();
Texture* getAttackAnimsTex();
Texture* getPlayerTex();
Texture* getTexture(int index);
void clearTextures();

#endif
