#include "common.h"
#include "objects_common.h"
#include "sfxeng.h"
#include "sndcache.h"
#include "weapons.h"
//shouldn't be here...
#include "characters_common.h"
#include "water.h"


Water::Water(string imagename, Uint16 xcord, Uint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname),
  au_water(sndcache->loadWAV("splash2.wav")) {
    weapon=Weapon(-1,W_WATER,WS_WATER);
}
Water::~Water() { }

void Water::enter(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        sfxeng->playWAV(au_water);
        ptr->setState(STATE_WATER);
        ptr->applySpeedMod(25);
        ptr->hit(DIR_ALL,weapon);
    }
}

void Water::leave(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        sfxeng->playWAV(au_water);
        ptr->applySpeedMod(400);
        ptr->unsetState(STATE_WATER);
    }
}
