#ifndef _FONT_H
#define _FONT_H 1

/*
Copyright (C) 2003 Matthias Braun

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** \brief Font structure

*/
typedef struct {
	SDL_Surface *surface;	
	int charPos[512];
	int maxPos;
} SFont_Font;

/** \brief Font engine.

    A simple font engine, called SFont, that uses special .pngs as fonts.
*/
class Font {
    public:
        Font(SDL_Surface* surface);
        ~Font();
        int getHeight() const;
        int getTextWidth(std::string text) const;
        void write(SDL_Surface* surface, std::string text, int x, int y) const;
        void writeCenter(SDL_Surface* surface, std::string text, int y) const;
    private:
        SFont_Font* initFont(SDL_Surface* Font);
        static Uint32 getPixel(SDL_Surface* surface, Sint32 X, Sint32 Y);
        SFont_Font* font;
};

#endif
