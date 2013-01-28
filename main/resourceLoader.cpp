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
void Start::loadImage(string filename) {
    cout << " -Loading " << filename << "..." << endl;

    Texture* newTexture = new Texture();
    newTexture->load("resources/" + filename);
    if (filename == "structures.png") {
        setStructureTex(newTexture);
        gotsStructureTex = true;
    } else if (filename == "menu.png") {
        setMenuTex(newTexture);
        gotsMenuTex = true;
    } else if (filename == "font.png") {
        setFontTex(newTexture);
        gotsFontTex = true;
    } else if (filename == "splatters.png") {
        setSplatterTex(newTexture);
        gotsSplatterTex = true;
    } else if (filename == "attackAnims.png") {
        setAttackAnimsTex(newTexture);
        gotsAttackAnimsTex = true;
    } else if (filename == "player.png") {
        setPlayerTex(newTexture);
        gotsPlayerTex = true;
    }
    addTexture(newTexture);
}
