#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "weapons.h"
#include "players_common.h"
#include "monsters_common.h"
#include "map.h"
#include "spike.h"

Spike::Spike(string imagename, Sint16 xcord, Sint16 ycord, Uint16 sdir, string oname):
  Object(imagename,xcord,ycord,oname),
  dir(sdir) {
    weapon=Weapon(-10,W_TOUCH,WS_SPIKE);
    otype|=(DIR_ALL&(~dir));
}
Spike::~Spike() { }

void Spike::touch(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        Uint16 adir=(curmap->getDirection(getCenter(),ptr->getCenter()));
        if (dir&adir) ptr->hit(adir,weapon);
    }
}
