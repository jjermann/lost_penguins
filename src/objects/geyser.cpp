#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
//shouldn't be here...
#include "vikings_common.h"
#include "map.h"
#include "geyser.h"


Geyser::Geyser(string imagename, Uint16 xcord, Uint16 ycord, Sint16 yAdd, string oname):
  Object(imagename,xcord,ycord,oname) {
    aspeed=yAdd;
    au_geyser=sndcache->loadWAV("geyser.wav");
    Deffect=0;
}
Geyser::~Geyser() { }

void Geyser::idle(Uint16 dt) {
    Object::idle(dt);
    Deffect+=dt;
    if (Deffect>T_GEYSIR) {
        Deffect=0;
        sfxeng->playWAV(au_geyser);
        std::set<Viking *> vikings=curmap->getVikingsIn(pos,true);
        std::set<Viking *>::iterator vkit=vikings.begin();
        while (vkit!=vikings.end()) {
            (*vkit)->addSpeed(aspeed);
            ++vkit;
        }
    }
}
