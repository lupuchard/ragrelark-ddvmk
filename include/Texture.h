/*=============================================================================
    Tim Jones
=============================================================================*/

#ifndef CTEXTURE_H
#define CTEXTURE_H

//=============================================================================

#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>

#include <GL/gl.h>
#include <GL/glu.h>

using namespace std;

//=============================================================================
class Texture {
    private:
        //GLuint     TextureID;

    public:
        int         Width;
        int         Height;
        GLuint     TextureID;

    public:
        Texture();

        //---------------------------------------------------------------------
        bool OnLoad(string File);

        bool OnLoad(SDL_Surface* Surface);

        void OnCleanup();

        //---------------------------------------------------------------------
        void Bind();

        //---------------------------------------------------------------------
        void RenderQuad(int X, int Y);

        void RenderQuad(int X, int Y, int Width, int Height);
};

//=============================================================================

#endif
