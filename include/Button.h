#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"
#include "Texture.h"
#include "RagDrawer.h"

enum ButtonState{UP, OVER, DOWN};

class Button {
    public:
        Button();
        Button(Coord c, int z, int width, int height, Texture* tex);

        void setPos(Coord c, int z);
        void setGraphics(Texture* tex, Coord in, int width, int height);
        void setText(int size, String text, Color textColor);
        void setFunc(boost::function<void(void)>);

        void render(RagDrawer* ragd);

        void mouse(MouseState* mouseState);
    private:
        Coord pos;
        int z;
        int width, height;

        Texture* tex;
        Coord inTex;

        int textSize;
        String text;
        Color textColor;

        boost::function<void(void)> func;

        ButtonState state;
};

#endif // BUTTON_H
