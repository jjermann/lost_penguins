#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "weapons.h"
#include "players_common.h"
#include "monsters_common.h"
#include "map.h"
#include "plant.h"

Plant::Plant(string imagename, Sint16 xcord, Sint16 ycord, Uint16 trecover, string cname):
  Monster(imagename,xcord,ycord,cname),
  recover(trecover),
  tcur(trecover) {
    maxhealth=1;
    weapon=Weapon(-1,W_TOUCH);
    au_hit=sndcache->loadWAV("creeper.wav");
    au_recover=au_hit;
    enemy_types|=(OTYPE_PLAYER);
}
Plant::~Plant() { }

void Plant::touch(Object* obj) {
    if (health==0) return;
    if(Player* ptr = dynamic_cast<Player*>(obj)) {
        Uint16 adir=(DIR_UP|curmap->getDirection(getCenter(),ptr->getCenter()));
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
            std::set<Character *> cset=curmap->getCharactersIn(enemy_types,pos,true);
            character_iterator cit=cset.begin();
            while (cit!=cset.end()) {
                (*cit)->hit(curmap->getDirection(pos,*(*cit)->getPos()),weapon);
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
