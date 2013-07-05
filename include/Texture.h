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

#include "Color.h"

#ifndef TEXTURE_H_H
#define TEXTURE_H_H

class Texture {
    public:
        /// Loads a new texture from that file. The given background will be made transparent.
        Texture(std::string file, Color background = NULL_COLOR);
        ~Texture();

        /// Binds the texture.
        void gl();

        /// Returns the width of the texture.
        int getWidth();

        /// Returns the height of the texture.
        int getHeight();
    private:
        int width;
        int height;
        GLuint id;
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

#endif // TEXTURE_H_H

#endif
