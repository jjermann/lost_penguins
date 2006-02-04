#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "weapons.h"
#include "players_common.h"
#include "monsters_common.h"
#include "scenario.h"
#include "plant.h"

Plant::Plant(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Monster(xcord,ycord,param) {
    enemy_types|=(OTYPE_PLAYER);
    weapon=Weapon(-1,W_TOUCH);
    tcur=recover;

    /* Parameters */
    if (!hasParam(parameters,"health")) health=min(1,(int)maxhealth);
    if (!hasParam(parameters,"audio_hit")) au_hit=scenario->sndcache->loadWAV("creeper.wav");
    if (hasParam(parameters,"audio_recover")) au_recover=scenario->sndcache->loadWAV(parameters["audio_recover"]);
      else au_recover=au_hit;
    if (hasParam(parameters,"time_recover")) recover=atoi(parameters["recover"].c_str());
      else recover=3000;
}
Plant::~Plant() { }

void Plant::touch(Object* obj) {
    if (health==0) return;
    if(Player* ptr = dynamic_cast<Player*>(obj)) {
        Uint16 adir=(DIR_UP|scenario->getDirection(getCenter(),ptr->getCenter()));
        if (!(adir&DIR_LR)) adir|=DIR_LEFT;
        ptr->hit(adir,weapon);
    }
}

void Plant::idle(Uint16 dt) {
    if (health==0) {
        tcur-=dt;
        if (tcur<=0) {
            sfxeng->playWAV(au_recover);
            tcur=recover;
            setHealth(maxhealth);
            std::set<Character *> cset=scenario->getCharactersIn(enemy_types,pos,true);
            character_iterator cit=cset.begin();
            while (cit!=cset.end()) {
                (*cit)->hit(scenario->getDirection(pos,*(*cit)->getPos()),weapon);
                ++cit;
            }
        }
    }
}

Uint16 Plant::hit(Uint16, Weapon& weap) {
    if (health==0) return health;
    sfxeng->playWAV(au_hit);
    return addHealth(weap.getDamage());
}
