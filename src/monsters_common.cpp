#include "common.h"
#include "animation.h"
#include "weapons.h"
#include "events.h"
#include "sfxeng.h"
#include "sndcache.h"
#include "monsters_common.h"
//shouldn't be here...
#include "scenario.h"
#include "players_common.h"


Monster::Monster(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Character(xcord,ycord,param),
  Dai(0),
  Dattack(0) {
    state=STATE_FALL;
    otype|=OTYPE_MONSTER;
    enemy_types|=OTYPE_PLAYER;
    dense_types|=OTYPE_PLAYER;

    /* Parameters */
    if (hasParam(parameters,"anim_left")) anim_left=loadAnimation(getParameters(parameters["anim_left"],':'));
      else anim_left.reset(new EmptyAnimation(&anim_right));
    if (hasParam(parameters,"anim_right")) anim_right=loadAnimation(getParameters(parameters["anim_right"],':'));
      else anim_right.reset(new EmptyAnimation(&anim_orig));
    if (!hasParam(parameters,"maxhealth")) maxhealth=1;
    if (!hasParam(parameters,"health")) health=min(1,(int)maxhealth);
    if (!hasParam(parameters,"maxspeedx")) maxspeedx=50;
    if (!hasParam(parameters,"maxspeedy")) maxspeedy=0;
    if (hasParam(parameters,"audio_hit")) au_hit=scenario->sndcache->loadWAV(parameters["audio_hit"]);
      else au_hit=scenario->sndcache->loadWAV("monhit.wav");

    animation.reset(new EmptyAnimation((state&STATE_LEFT) ? &anim_left : &anim_right));
}

Monster::~Monster() {
}

void Monster::addEnter(std::set<Object *>& aset) {
    Character::addEnter(aset);    
}

void Monster::addTouch(std::set<Object *>& aset) {
    Character::addTouch(aset);
    Player* ptr=NULL;
    object_iterator obit=aset.begin();
    while (obit != aset.end()) {
        if ((*obit)->getType()&enemy_types) {
            if((ptr = dynamic_cast<Player*>(*obit))) {
                setState(STATE_ATTACK);
                targets.insert(ptr);
            }
        }
        ++obit;
    }
}
void Monster::removeEnter(std::set<Object *>& rset) {
    Character::removeEnter(rset);
    unsetState(STATE_ATTACK);
}
void Monster::removeTouch(std::set<Object *>& rset) {
    Character::removeTouch(rset);
    Player* ptr=NULL;
    object_iterator obit=rset.begin();
    while (obit != rset.end()) {
        if ((*obit)->getType()&OTYPE_PLAYER) {
            if((ptr = dynamic_cast<Player*>(*obit))) {
                targets.erase(ptr);
            }
        }
        ++obit;
    }

    if (targets.empty()) unsetState(STATE_ATTACK);
}

void Monster::removedObject(Object* obj) {
    Character::removedObject(obj);
    if (Player* ptr = dynamic_cast<Player*>(obj)) {
        targets.erase(ptr);
    }
}

void Monster::updateAnimState() {
    if (state&STATE_LEFT) {
        setAnim(anim_left);   
    } else {   
        setAnim(anim_right);
    }
}

void Monster::idle(Uint16 dt) {
    Character::idle(dt);
    runAI(dt);
}

Hit Monster::move(Uint16 dt, bool check) {
    return Character::move(dt,check);
}

void Monster::fall(Uint16 dt) {
    Character::fall(dt);
}

void Monster::crash(Uint16 dir) {
    if (event) event->cancel();
    switch (dir) {
        case DIR_DOWN: {
            unsetState(STATE_ACT_1);
            unsetState(STATE_ACT_2);
            unsetState(STATE_ACT_3);
            //should they be hit as well??
            break;
        }
        default: { }
    }
    Character::crash(dir);
}

void Monster::die() {
    Character::die();
}

Uint16 Monster::hit(Uint16, Weapon& weap) {
    sfxeng->playWAV(au_hit);
    Uint16 newhealth=addHealth(weap.getDamage());
    if (newhealth==0) die();
    return newhealth;
}
