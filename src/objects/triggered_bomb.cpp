#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "weapons.h"
#include "characters_common.h"
#include "map.h"
#include "triggered_bomb.h"

TriggeredBomb::TriggeredBomb(string imagename, Uint16 xcord, Uint16 ycord, Uint16 tleft, string cname):
  Character(imagename,xcord,ycord,cname), countdown(tleft) {
    weapon=Weapon(-1,W_EXPLOSION);
    au_bomb=sndcache->loadWAV("explsn.wav");
}
TriggeredBomb::~TriggeredBomb() { }

void TriggeredBomb::idle(Uint16 dt) {
    countdown-=dt;
    if (countdown<=0) {
        cout << "Booom!\n";
        sfxeng->playWAV(au_bomb);
        std::set<Character *> cset=curmap->getCharactersIn(pos,true,20);
        character_iterator cit=cset.begin();
        while (cit!=cset.end()) {
            (*cit)->hit(curmap->getDirection(pos,*(*cit)->getPos()),weapon);
            ++cit;
        }
        die();
    }
}
