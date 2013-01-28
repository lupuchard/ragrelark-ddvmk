#include "Texture.h"

vector<Texture*> textures;
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

Texture::Texture() {
    textureID = 0;
    width   = 0;
    height  = 0;
}

void Texture::bind() {
    glBindTexture(GL_TEXTURE_2D, textureID);
}

bool Texture::load(string file) {
    SDL_Surface* tempSurf = IMG_Load(file.c_str());
    if (!tempSurf) {
        cout << "Texture file not found: " << file << endl;
        return false;
    }
    load(tempSurf);
    SDL_FreeSurface(tempSurf);
    return true;
}

bool Texture::load(SDL_Surface* surface) {
    cleanup();

    if(!surface) return false;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    int mode;
    if(surface->format->BytesPerPixel == 4) {
        mode = GL_RGBA;
    } else {
        mode = GL_RGB;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, mode);
    glTexImage2D(GL_TEXTURE_2D, 0, mode, surface->w, surface->h, 0, mode, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    width   = surface->w;
    height  = surface->h;

    return true;
}

void Texture::cleanup() {
    if(textureID > 0) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }
}
