#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
#include "scenario.h"
#include "door.h"


Door::Door(string imagename, Sint16 xcord, Sint16 ycord, string keyname, string oname):
  Object(imagename,xcord,ycord,oname),
  open(false),
  key(keyname),
  au_open(scenario->sndcache->loadWAV("dooropn.wav")),
  au_close(au_open) {
    otype=OTYPE_DENSE;
}
Door::~Door() { }

bool Door::act(Object* obj) {
    //sanity check
    if (!(obj && obj->getName()==key)) return false;
    if (!(scenario->getCharactersIn(OTYPE_PLAYER|OTYPE_MONSTER,pos,true).empty())) return false;

    //switch state
    if (open) {
        sfxeng->playWAV(au_close);
        open=false;
    } else {
        sfxeng->playWAV(au_open);
        open=true;
    }
    
    //TODO: check if closing is ok
    //open or close the door
    if (open) otype=NOTHING;
    else otype=OTYPE_DENSE;

    return true;
}
