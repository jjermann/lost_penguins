#include "common.h"
#include "sfxeng.h"
#include "objects_common.h"
//this shouldn't be here
#include "players_common.h"
#include "scenario.h"
#include "imgcache.h"
#include "sndcache.h"
#include "key.h"


Key::Key(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Item(xcord,ycord,param) {
    /* Parameters */
    if (!hasParam(parameters,"image")) anim_orig=loadAnimation(scenario->imgcache->loadImage(1,"key.bmp"));
    if (hasParam(parameters,"audio_key")) au_key=scenario->sndcache->loadWAV(parameters["audio_key"]);
      else au_key=NULL;
}
Key::~Key() { }

bool Key::act(Object* obj) {
    if ((obj==NULL) || (obj!=owner)) return false;
    std::set<Object *> enter=owner->getEnter();
    object_iterator i=enter.begin();
    while (i!=enter.end()) {
        if ((*i)->act(this)) {
            sfxeng->playWAV(au_key);
            owner->removeItem();
            return true;
        } else ++i;
    }
    return false;
}
