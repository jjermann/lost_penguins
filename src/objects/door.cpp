#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
#include "scenario.h"
#include "door.h"


Door::Door(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Object(xcord,ycord,param) {
    otype=OTYPE_DENSE;

    /* Parameters */
    if (hasParam(parameters,"key")) key=parameters["key"];
      else key="Key";
    if (hasParam(parameters,"audio_open"))  au_open =scenario->sndcache->loadWAV(parameters["audio_open"]);
      else au_open=scenario->sndcache->loadWAV("dooropn.wav");
    if (hasParam(parameters,"audio_close")) au_close=scenario->sndcache->loadWAV(parameters["audio_close"]);
      else au_close=scenario->sndcache->loadWAV("dooropn.wav");
    if (hasParam(parameters,"start_open"))  open=true;
      else open=false;
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
