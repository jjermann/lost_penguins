#include "common.h"
#include "menu.h"

ImageCache* imgcache;
SoundCache* sndcache;
GraphicsEngine* gfxeng;
SoundsEngine* sfxeng;
Scenario* scenario;
InputHandler* input;
Font* font;
Font* font2;
Config config;
Uint8 game_mode=NOTHING;
Menu* menu=NULL;
Box* box=NULL;
Editor* editor=NULL;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const Uint32 rmask=0xff000000;
const Uint32 gmask=0x00ff0000;
const Uint32 bmask=0x0000ff00;
const Uint32 amask=0x000000ff;
#else
const Uint32 rmask=0x000000ff;
const Uint32 gmask=0x0000ff00;
const Uint32 bmask=0x00ff0000;
const Uint32 amask=0xff000000;
#endif
Uint32 vflags=SDL_HWSURFACE|SDL_RESIZABLE|SDL_DOUBLEBUF;

string itos(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}

int boost(int a, int b) {
    if (a>=0) return a=max(0,a+b);
    else return a=min(0,a-b);
}

void setGameMode(Uint8 newmode) {
    game_mode=newmode;
    if (game_mode&GAME_EDIT && !(game_mode&GAME_MENU)) {
        SDL_ShowCursor(SDL_ENABLE);
    } else {
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void addGameMode(Uint8 addmode) {
    game_mode|=addmode;
    if (game_mode&GAME_EDIT && !(game_mode&GAME_MENU)) {
        SDL_ShowCursor(SDL_ENABLE);
    } else {
        SDL_ShowCursor(SDL_DISABLE);
    }
}

void removeGameMode(Uint8 rmmode) {
    game_mode&=~rmmode;
    if (game_mode&GAME_EDIT && !(game_mode&GAME_MENU)) {
        SDL_ShowCursor(SDL_ENABLE);
    } else {
        SDL_ShowCursor(SDL_DISABLE);
    }
}
