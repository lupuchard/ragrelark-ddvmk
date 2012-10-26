//=============================================================================
#include "Texture.h"

//=============================================================================
Texture::Texture() {
    TextureID = 0;

    Width   = 0;
    Height  = 0;
}

//=============================================================================
bool Texture::OnLoad(string File) {
    SDL_Surface* tempSurf = NULL;

    if((tempSurf = IMG_Load(File.c_str())) == NULL) {
        cout << "Unable to load file : " << File << endl;
        return false;
    }

    OnLoad(tempSurf);

    SDL_FreeSurface(tempSurf);

    return true;
}

//-----------------------------------------------------------------------------
bool Texture::OnLoad(SDL_Surface* Surface) {
    OnCleanup();

    if(Surface == NULL) {
        return false;
    }

    glGenTextures(1, &TextureID);

    glBindTexture(GL_TEXTURE_2D, TextureID);

    int Mode = GL_RGB;

    if(Surface->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, Mode);

    glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Width   = Surface->w;
    Height  = Surface->h;

    return true;
}

//-----------------------------------------------------------------------------
void Texture::OnCleanup() {
    if(TextureID > 0) {
        glDeleteTextures(1, &TextureID);
        TextureID = 0;
    }
}

//=============================================================================
void Texture::Bind() {
    glBindTexture(GL_TEXTURE_2D, TextureID);
}

//=============================================================================
void Texture::RenderQuad(int X, int Y) {
    RenderQuad(X, Y, Width, Height);
}

//------------------------------------------------------------------------------
void Texture::RenderQuad(int X, int Y, int Width, int Height) {
    Bind();

    //This combination is matched with the current Ortho view
    //Don't change the Ortho view, otherwise textures will be flipped
    //or rotated
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(X, Y, 0);
        glTexCoord2f(1, 0); glVertex3f(X + Width, Y, 0);
        glTexCoord2f(1, 1); glVertex3f(X + Width, Y + Height, 0);
        glTexCoord2f(0, 1); glVertex3f(X, Y + Height, 0);
    glEnd();
}

//=============================================================================
