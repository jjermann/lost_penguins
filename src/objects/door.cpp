#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
#include "map.h"
#include "door.h"


Door::Door(string imagename, Uint16 xcord, Uint16 ycord, string keyname, string oname):
  Object(imagename,xcord,ycord,oname) {
    open=false;
    otype=OTYPE_DENSE;
    key=keyname;
    au_open=sndcache->loadWAV("dooropn.wav");
    au_close=au_open;
}
Door::~Door() { }

bool Door::act(Object* obj) {
    //sanity check
    if (!(obj && obj->getName()==key)) return false;
    if (!(curmap->getCharactersIn(pos,true).empty())) return false;

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