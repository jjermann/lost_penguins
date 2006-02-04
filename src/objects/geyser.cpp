#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
//shouldn't be here...
#include "players_common.h"
#include "scenario.h"
#include "geyser.h"


Geyser::Geyser(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Object(xcord,ycord,param),
  Deffect(0) {
    /* Parameters */
    if (hasParam(parameters,"aspeed")) aspeed=atoi(parameters["aspeed"].c_str());
      else aspeed=0;
    if (hasParam(parameters,"audio_geyser")) au_geyser=scenario->sndcache->loadWAV(parameters["audio_geyser"]);
      else au_geyser=scenario->sndcache->loadWAV("geyser.wav");
}
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
