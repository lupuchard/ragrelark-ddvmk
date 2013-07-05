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

#include "Texture.h"

std::vector<Texture*> textures;
Texture* structureTex;
Texture* menuTex;
Texture* fontTex;
Texture* splatterTex;
Texture* attackAnimsTex;
Texture* playerTex;

int addTexture(Texture* texture) {
    textures.push_back(texture);
    return textures.size() - 1;
}
void setStructureTex(Texture* texture) { structureTex = texture; }
void setMenuTex(Texture* texture) { menuTex = texture; }
void setFontTex(Texture* texture) { fontTex = texture; }
void setSplatterTex(Texture* texture) { splatterTex = texture; }
void setAttackAnimsTex(Texture* texture) { attackAnimsTex = texture; }
void setPlayerTex(Texture* texture) { playerTex = texture; }
Texture* getStructureTex() { return structureTex; }
Texture* getMenuTex() { return menuTex; }
Texture* getFontTex() { return fontTex; }
Texture* getSplatterTex() { return splatterTex; }
Texture* getAttackAnimsTex() { return attackAnimsTex; }
Texture* getPlayerTex() { return playerTex; }
Texture* getTexture(int index) {
    return textures[index];
}
void clearTextures() {
    for (unsigned int i = 0; i < textures.size(); i++) {
        delete textures[i];
    }
    textures.clear();
}

Texture::Texture(std::string file, Color back) {
    SDL_Surface* surface = IMG_Load(file.c_str());
    if (!surface) {
        std::cout << "File not found or not an image: " << file << std::endl;
    } else {
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);

        width = surface->w;
        height = surface->h;

        int bytesPerPixel = surface->format->BytesPerPixel;
        if(bytesPerPixel == 4) {
            //std::cout << "Loading image \"" << file << "\" as RGBA." << std::endl;
            glPixelStorei(GL_UNPACK_ALIGNMENT, GL_RGBA);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        } else if (bytesPerPixel == 3) {
            //std::cout << "Loading image \"" << file << "\" as RGB." << std::endl;
            glPixelStorei(GL_UNPACK_ALIGNMENT, GL_RGB);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);
        } else if (surface->format->BitsPerPixel <= 8 ) {
            //std::cout << "Loading image \"" << file << "\" as indexed." << std::endl;
            //assuming it's a palette, and converting it to rgba

            int paletteSize = surface->format->palette->ncolors;
            float reds[paletteSize];
            float greens[paletteSize];
            float blues[paletteSize];
            float alphas[paletteSize];
            for (int i = 0; i < paletteSize; i++) {
                reds[i] = surface->format->palette->colors[i].r;
                greens[i] = surface->format->palette->colors[i].g;
                blues[i] = surface->format->palette->colors[i].b;
                if (back.red == reds[i] || back.green == greens[i] || back.blue == blues[i]) {
                    alphas[i] = 0;
                } else alphas[i] = 255;
            }

            glPixelMapfv(GL_PIXEL_MAP_I_TO_R, paletteSize, reds);
            glPixelMapfv(GL_PIXEL_MAP_I_TO_G, paletteSize, greens);
            glPixelMapfv(GL_PIXEL_MAP_I_TO_B, paletteSize, blues);
            glPixelMapfv(GL_PIXEL_MAP_I_TO_A, paletteSize, alphas);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, surface->pixels);
        } else {
            std::cout << "I don't know how to handle this image: " << file << " ... it seems to have " << (int)surface->format->BitsPerPixel << " bits per pixel?" << std::endl;
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        SDL_FreeSurface(surface);
    }
}

Texture::~Texture() {
    if(id > 0) {
        glDeleteTextures(1, &id);
        id = 0;
    }
}

void Texture::gl() {
    glBindTexture(GL_TEXTURE_2D, id);
}

int Texture::getWidth() {
    return width;
}

int Texture::getHeight() {
    return height;
}
