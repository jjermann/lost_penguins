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
bool paused=false;
bool running=false;
Menu* menu;

string itos(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}

int boost(int a, int b) {
    if (a>=0) return a=max(0,a+b);
    else return a=min(0,a-b);
}
