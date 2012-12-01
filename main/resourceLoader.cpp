#include "Start.h"

#define RESOURCES_LOC "resources/"

void Start::buildFont() {
    Display *dpy;
    XFontStruct *fontInfo;
    base = glGenLists(96);
    dpy = XOpenDisplay(NULL);
    fontInfo = XLoadQueryFont(dpy, "-adobe-helvetica-medium-r-normal--18-*-*-*-p-*-iso8859-1" );
    if (fontInfo == NULL) {
        fontInfo = XLoadQueryFont(dpy, "fixed");
        if (fontInfo == NULL) {
            cout << "no X font avaliable?" << endl;
        }
    }
    glXUseXFont(fontInfo->fid, 32, 96, base);
    XFreeFont(dpy, fontInfo);
    XCloseDisplay(dpy);
}

Texture* Start::getTexture(int i) {
    return textures[i];
}

void Start::loadImage(string filename) {
    cout << " -Loading " << filename << "..." << endl;

    Texture* newTexture = new Texture();
    newTexture->OnLoad("resources/" + filename);
    if (filename == "structures.png") {
        structureTex = newTexture;
        gotsStructureTex = true;
    } else if (filename == "menu.png") {
        menuTex = newTexture;
        gotsMenuTex = true;
    } else if (filename == "font.png") {
        fontTex = newTexture;
        gotsFontTex = true;
    } else if (filename == "splatters.png") {
        splatterTex = newTexture;
        gotsSplatterTex = true;
    } else if (filename == "attackAnims.png") {
        attackAnimsTex = newTexture;
        gotsAttackAnimsTex = true;
    } else if (filename == "player.png") {
        playerTex = newTexture;
        gotsPlayerTex = true;
    }
    textures.push_back(newTexture);
}
