#include "common.h"

ImageCache* imgcache;
SoundCache* sndcache;
GraphicsEngine* gfxeng;
SoundsEngine* sfxeng;
ObjectsPool* pool;
Player* player;
Background* background;
Map* curmap;
InputHandler* input;
AnimHandler* anim;
Font* font;
Font* font2;
SDL_Rect* maparea;
Config config;
bool paused=false;
bool failed=false;
bool fps=true;

//convert int to string
string itos(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}
