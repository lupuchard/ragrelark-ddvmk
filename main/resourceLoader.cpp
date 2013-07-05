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
            std::cout << "no X font avaliable?" << std::endl;
        }
    }
    glXUseXFont(fontInfo->fid, 32, 96, base);
    XFreeFont(dpy, fontInfo);
    XCloseDisplay(dpy);
}
void Start::loadImage(std::string filename) {
    std::cout << " -Loading " << filename << "..." << std::endl;
    Texture* newTexture = new Texture("resources/" + filename);
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
