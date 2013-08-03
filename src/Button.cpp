#include "Button.h"

Button::Button() {
    z = 0;
    width = 0;
    height = 0;
    tex = NULL;
    textSize = -1;
    state = UP;
    func = NULL;
}

Button::Button(Coord c, int z, int width, int height, Texture* g) {
    pos = c;
    this->z = z;
    this->width = width;
    this->height = height;
    tex = g;
    func = NULL;
    textSize = -1;
    state = UP;
}

void Button::setPos(Coord c, int z) {
    pos = c;
    this->z = z;
}

void Button::setGraphics(Texture* tex, Coord in, int w, int h) {
    this->tex = tex;
    inTex = in;
    width = w;
    height = h;
}

void Button::setText(int size, String text, Color tColor) {
    textSize = size;
    this->text = text;
    textColor = tColor;
}

void Button::setFunc(boost::function<void(void)> f) {
    func = f;
}

void Button::render(RagDrawer* ragd) {
    if (tex) {
        switch(state) {
            case UP:   ragd->drawTileSuperSpe(pos.x - width / 2, pos.y - height / 2, z, width, height, tex, inTex.x, inTex.y             , width, height); break;
            case OVER: ragd->drawTileSuperSpe(pos.x - width / 2, pos.y - height / 2, z, width, height, tex, inTex.x, inTex.y + height    , width, height); break;
            case DOWN: ragd->drawTileSuperSpe(pos.x - width / 2, pos.y - height / 2, z, width, height, tex, inTex.x, inTex.y + height * 2, width, height); break;
            default: break;
        }
    }
    if (textSize > -1) {
        ragd->renderText(text, textSize, pos.x, pos.y - ragd->getTextSizeHeight(textSize) / 2, z + 1, CENTER, textColor);
    }
}

void Button::mouse(MouseState* s) {
    bool over = (s->pos - pos + Coord(width / 2, height / 2)).inBounds(width, height);
    if (over) {
        if (state == UP) state = OVER;
        else if (state == OVER && s->leftPress) {
            state = DOWN;
            if (func) func();
        } else if (state == DOWN && !s->leftDown) state = OVER;
    } else {
        state = UP;
    }
}
