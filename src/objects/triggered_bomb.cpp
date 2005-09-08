#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "weapons.h"
#include "characters_common.h"
#include "scenario.h"
#include "triggered_bomb.h"

TriggeredBomb::TriggeredBomb(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Character(xcord,ycord,parameters) {
    weapon=Weapon(-1,W_EXPLOSION);
    enemy_types|=(OTYPE_PLAYER|OTYPE_MONSTER);

    /* Parameters */
    if (hasParam(parameters,"audio_bomb")) au_bomb=scenario->sndcache->loadWAV(parameters["audio_bomb"]);
      else au_bomb=scenario->sndcache->loadWAV("explsn.wav");
    if (hasParam(parameters,"countdown")) countdown=atoi(parameters["countdown"].c_str());
      else countdown=3000;
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
