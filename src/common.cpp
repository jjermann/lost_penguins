#include "lost_vikings.h"

GraphicsEngine* gfxeng;
SoundsEngine* sfxeng;
ObjectsPool* pool;
Viking* viking;
Background* background;
Map* map;
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

//Speed stuff
Speed& Speed::operator+=(const Speed& aspeed) {
    x+=aspeed.x; 
    y+=aspeed.y; 
    return *this;
}
Speed& Speed::operator-=(const Speed& aspeed) {
    x-=aspeed.x; 
    y-=aspeed.y; 
    return *this;
}
Speed& Speed::operator*=(const Speed& aspeed) {
    x*=aspeed.x; 
    y*=aspeed.y; 
    return *this;
}
Speed& Speed::operator/=(const Speed& aspeed) {
    x/=aspeed.x; 
    y/=aspeed.y; 
    return *this;
}
Speed& Speed::operator*=(Sint16 mult) {
    x*=mult;
    y*=mult;
    return *this;
}
Speed& Speed::operator/=(Sint16 mult) {
    x/=mult;
    y/=mult;
    return *this;
}
Speed operator-(const Speed& oldspeed) {
    Speed sp=oldspeed;
    sp.x*=-1;
    sp.y*=-1;
    return (sp);
}
