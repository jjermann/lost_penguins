#include "common.h"
#include "objects_common.h"
#include "sfxeng.h"
#include "sndcache.h"
#include "weapons.h"
//shouldn't be here...
#include "map.h"
#include "characters_common.h"
#include "water.h"


Water::Water(string imagename, Sint16 xcord, Sint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname),
  au_water(scenario->sndcache->loadWAV("splash2.wav")) {
    weapon=Weapon(-1,W_WATER,WS_WATER);
}
Water::~Water() { }

void Water::enter(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        sfxeng->playWAV(au_water);
        ptr->setState(STATE_WATER);
        ptr->addMaxSpeed(-200);
        ptr->hit(DIR_ALL,weapon);
    }
}

void Water::leave(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        sfxeng->playWAV(au_water);
        ptr->addMaxSpeed(200);
        ptr->unsetState(STATE_WATER);
    }
}
