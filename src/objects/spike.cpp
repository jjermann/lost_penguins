#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "weapons.h"
#include "players_common.h"
#include "monsters_common.h"
#include "scenario.h"
#include "spike.h"

Spike::Spike(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Object(xcord,ycord,param) {
    weapon=Weapon(-10,W_TOUCH,WS_SPIKE);

    /* Parameters */
    if (hasParam(parameters,"direction")) dir=getDirFromString(parameters["direction"]);
      else dir=DIR_UP;
    otype|=(DIR_ALL&(~dir));
}
Spike::~Spike() { }

void Spike::touch(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        Uint16 adir=(scenario->getDirection(getCenter(),ptr->getCenter()));
        if (dir&adir) ptr->hit(adir,weapon);
    }
}
