#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "weapons.h"
#include "characters_common.h"
#include "map.h"
#include "triggered_bomb.h"

TriggeredBomb::TriggeredBomb(string imagename, Sint16 xcord, Sint16 ycord, Uint16 tleft, string cname):
  Character(imagename,xcord,ycord,cname),
  countdown(tleft),
  au_bomb(scenario->sndcache->loadWAV("explsn.wav")) {
    weapon=Weapon(-1,W_EXPLOSION);
    enemy_types|=(OTYPE_PLAYER|OTYPE_MONSTER);
}
TriggeredBomb::~TriggeredBomb() { }

void TriggeredBomb::idle(Uint16 dt) {
    countdown-=dt;
    if (countdown<=0) {
        cout << "Booom!\n";
        sfxeng->playWAV(au_bomb);
        std::set<Character *> cset=scenario->getCharactersIn(enemy_types,pos,true,20);
        character_iterator cit=cset.begin();
        while (cit!=cset.end()) {
            (*cit)->hit(scenario->getDirection(pos,*(*cit)->getPos()),weapon);
            ++cit;
        }
        die();
    }
}
