#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
//shouldn't be here...
#include "players_common.h"
#include "map.h"
#include "geyser.h"


Geyser::Geyser(string imagename, Sint16 xcord, Sint16 ycord, Sint16 yAdd, string oname):
  Object(imagename,xcord,ycord,oname),
  aspeed(yAdd),
  au_geyser(scenario->sndcache->loadWAV("geyser.wav")),
  Deffect(0) { }
Geyser::~Geyser() { }

void Geyser::idle(Uint16 dt) {
    Object::idle(dt);
    Deffect+=dt;
    if (Deffect>T_GEYSIR) {
        Deffect=0;
        sfxeng->playWAV(au_geyser);
        std::set<Character *> characters=scenario->getCharactersIn(OTYPE_PLAYER,pos,true);
        std::set<Character *>::iterator chrit=characters.begin();
        while (chrit!=characters.end()) {
            (*chrit)->addSpeed(aspeed);
            ++chrit;
        }
    }
}
