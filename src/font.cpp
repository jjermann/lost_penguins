/*  SFont: a simple font-library that uses special .pngs as fonts
    Copyright (C) 2003 Karl Bartel

    License: GPL or LGPL (at your choice)
    WWW: http://www.linux-games.com/sfont/

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
                                                                                
    Karl Bartel
    Cecilienstr. 14                                                    
    12307 Berlin
    GERMANY
    karlb@gmx.net                                                      
*/                                                                            

#include <iostream>
#include <SDL.h>
#include <assert.h>
#include <string>
#include "font.h"

using std::string;

Font::Font(SDL_Surface* surface) {
    font = initFont(surface);
    if(!font) std::cout << "Couldn't initialize font!";
}

Font::~Font() {
    free(font);
}
            
int Font::getHeight() const {
    return (font->surface->h - 1);
}

int Font::getTextWidth(string text) const {
    const char* c;
    int charoffset=0;
    int width = 0;

    if(text.c_str() == NULL) return 0;

    for(c = text.c_str(); *c != '\0'; c++) {
        charoffset = ((int) *c - 33) * 2 + 1;
        // skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > font->maxPos) {
            width += font->charPos[2]-font->charPos[1];
            continue;
         }
 
        width += font->charPos[charoffset+1] - font->charPos[charoffset];
    }

    return width;
}

void Font::write(SDL_Surface *surface, string text, int x, int y) const {
    const char* c;
    int charoffset;
    SDL_Rect srcrect, dstrect;

    if(text.c_str() == NULL) return;

    // these values won't change in the loop
    srcrect.y = 1;
    dstrect.y = y;
    srcrect.h = dstrect.h = font->surface->h - 1;

    for(c = text.c_str(); *c != '\0' && x <= surface->w ; c++) {
        charoffset = ((int) (*c - 33)) * 2 + 1;
        // skip spaces and nonprintable characters
        if (*c == ' ' || charoffset < 0 || charoffset > font->maxPos) {
            x += font->charPos[2]-font->charPos[1];
            continue;
        }

        srcrect.w = dstrect.w = 
          (font->charPos[charoffset+2] + font->charPos[charoffset+1])/2 -
          (font->charPos[charoffset] + font->charPos[charoffset-1])/2;
        srcrect.x = (font->charPos[charoffset]+font->charPos[charoffset-1])/2;
        dstrect.x = x - (Uint32)((font->charPos[charoffset] - font->charPos[charoffset-1])/2);

        SDL_BlitSurface(font->surface, &srcrect, surface, &dstrect); 

        x += font->charPos[charoffset+1] - font->charPos[charoffset];
    }
}

void Font::writeCenter(SDL_Surface *surface, string text, int y) const {
    write(surface, text, surface->w/2 - getTextWidth(text)/2, y);
}

void Font::writeCenter(SDL_Surface *surface, string text, int x, int y) const {
    write(surface, text, x - getTextWidth(text)/2, y);
}

Uint32 Font::getPixel(SDL_Surface *surface, Sint32 X, Sint32 Y) {
    Uint8  *bits;
    Uint32 Bpp;

    assert(X>=0);
    assert(X<surface->w);
   
    Bpp = surface->format->BytesPerPixel;
    bits = ((Uint8 *)surface->pixels)+Y*surface->pitch+X*Bpp;

    // Get the pixel
    switch(Bpp) {
        case 1:
            return *((Uint8 *)surface->pixels + Y * surface->pitch + X);
            break;
        case 2:
            return *((Uint16 *)surface->pixels + Y * surface->pitch/2 + X);
            break;
        case 3: { // Format/endian independent 
             Uint8 r, g, b;
             r = *((bits)+surface->format->Rshift/8);
             g = *((bits)+surface->format->Gshift/8);
             b = *((bits)+surface->format->Bshift/8);
             return SDL_MapRGB(surface->format, r, g, b);
             break;
        }
        case 4:
            return *((Uint32 *)surface->pixels + Y * surface->pitch/4 + X);
            break;
    }

//   return -1;
    return 0;
}

SFont_Font* Font::initFont(SDL_Surface* surface) {
    int x = 0, i = 0;
    Uint32 pixel;
    SFont_Font* Font;
    Uint32 pink;

    if (surface == NULL) return NULL;

    Font = (SFont_Font *) malloc(sizeof(SFont_Font));
    Font->surface = surface;

    SDL_LockSurface(surface);

    pink = SDL_MapRGB(surface->format, 255, 0, 255);
    while (x < surface->w) {
        if (getPixel(surface, x, 0) == pink) { 
            Font->charPos[i++]=x;
            while((x < surface->w) && (getPixel(surface, x, 0)== pink)) x++;
            Font->charPos[i++]=x;
        }
        x++;
    }
    Font->maxPos = x-1;
    
    pixel = getPixel(surface, 0, surface->h-1);
    SDL_UnlockSurface(surface);
    SDL_SetColorKey(surface, SDL_SRCCOLORKEY, pixel);

    return Font;
}
