#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
//shouldn't be here...
#include "players_common.h"
#include "map.h"
#include "geyser.h"


Geyser::Geyser(string imagename, Uint16 xcord, Uint16 ycord, Sint16 yAdd, string oname):
  Object(imagename,xcord,ycord,oname),
  aspeed(yAdd),
  au_geyser(sndcache->loadWAV("geyser.wav")),
  Deffect(0) { }
Geyser::~Geyser() { }

void Geyser::idle(Uint16 dt) {
    Object::idle(dt);
    Deffect+=dt;
    if (Deffect>T_GEYSIR) {
        Deffect=0;
        sfxeng->playWAV(au_geyser);
        std::set<Player *> players=curmap->getPlayersIn(pos,true);
        std::set<Player *>::iterator plit=players.begin();
        while (plit!=players.end()) {
            (*plit)->addSpeed(aspeed);
            ++plit;
        }
    }
}
