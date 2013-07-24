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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "main.h"
#include "Color.h"

class Texture {
    public:
        /// Loads a new texture from that file. The given background will be made transparent.
        Texture(String file, Color background = NULL_COLOR);
        ~Texture();

        /// Binds the texture.
        void gl();

        /// Returns the width of the texture.
        int getWidth();
        /// Returns the height of the texture.
        int getHeight();

        int getIndex();

        //statics
        static Texture* get(int index);
        static Texture* get(String filename);

        static void clear();
    private:
        int width;
        int height;
        GLuint id;
        int index;

        //statics
        static std::vector<Texture*> textures;
        static std::map<String, Texture*> textureFileMap;
};

#endif // TEXTURE_H
