#include "common.h"
#include "events.h"
#include "anim.h"
#include "weapons.h"
#include "scenario.h"
#include "sfxeng.h"
#include "characters_common.h"


Event::Event(Object* obj, Uint16 length, Uint16 edelay, Uint32 switchstate):
  owner(obj),
  duration(length),
  delay(edelay),
  tleft(delay),
  started(false),
  state(switchstate|ESTATE_RUN) { }
Event::~Event() {
    owner->clearEvents();
}
Uint16 Event::update(Uint16 dt) {
    Uint16 evstate=NOTHING;
    tleft-=dt;
    if (tleft<=0) {
        //end
        if (started) {
            tleft=0;
            evstate=EV_END;
        //start
        } else {
            tleft=duration;
            evstate=EV_START;
        }
    //running
    } else if (started) {
        evstate=EV_RUN;
    //delayed
    } else {
        evstate=EV_DELAY;
    }
    return evstate;
}
void Event::start() {
    owner->switchState(state);
    started=true;
}
void Event::end() {
    if (started) owner->switchState(state);
    delete this;
}
void Event::cancel() {
    if (started) owner->switchState(state);
    delete this;
}

AnimEvent::AnimEvent(Object* obj, Uint16 length, Uint16 edelay, Uint32 switchstate, Mix_Chunk* asound, Animation* runanim, bool delanim):
  Event(obj,length,edelay,switchstate),
  anim(runanim),
  del(delanim),
  sound(asound) {
    if (anim) state|=ESTATE_ANIM;
    if (anim!=NULL) duration=30000;
}
Uint16 AnimEvent::update(Uint16 dt) {
    Uint16 evstate=Event::update(dt);
    if (started && anim && (!(anim->isRunning()))) return EV_END;
    else return evstate; 
}
void AnimEvent::start() {
    sfxeng->playWAV(sound);
    if (anim) owner->setAnim(anim);
    Event::start();
}
void AnimEvent::end() {
    if (started && anim) {
        if (del) delete anim; 
        owner->resetAnimState();
    }
    Event::end();
}
void AnimEvent::cancel() {
    if (started && anim) {
        if (del) delete anim;
        owner->resetAnimState();
    }
    Event::cancel();
}


CEvent::CEvent(Character* chr, Uint16 length, Uint16 edelay, Uint32 switchstate):
  Event(chr,length,edelay,switchstate),
  charowner(chr) { }
  
//Exactly the same as AnimEvent with Character* instead of Object*
CAnimEvent::CAnimEvent(Character* chr, Uint16 length, Uint16 edelay, Uint32 switchstate, Mix_Chunk* asound, Animation* runanim, bool delanim):
  CEvent(chr,length,edelay,switchstate),
  anim(runanim),
  del(delanim),
  sound(asound) {
    if (anim) state|=ESTATE_ANIM;
    if (anim!=NULL) duration=30000;
}
Uint16 CAnimEvent::update(Uint16 dt) {
    Uint16 evstate=CEvent::update(dt);
    if (started && anim && (!(anim->isRunning()))) return EV_END;
    else return evstate;
}
void CAnimEvent::start() {
    sfxeng->playWAV(sound);
    if (anim) charowner->setAnim(anim);
    CEvent::start();
}
void CAnimEvent::end() {
    if (started && anim) {
        if (del) delete anim;
        charowner->resetAnimState();
    }
    CEvent::end();
}
void CAnimEvent::cancel() {
    if (started && anim) { 
        if (del) delete anim;
        charowner->resetAnimState();
    }
    CEvent::cancel();
}


ERun::ERun(Character* chr, Uint16 length, Sint16 edmax, Uint16 edelay, Uint32 switchstate, Mix_Chunk* esound, Animation* runanim, bool delanim):
  CAnimEvent(chr,length,edelay,(switchstate|STATE_PRESS_LR),esound,runanim,delanim),
  dmax(edmax),
  t_reset(0) {
    charowner->setState(STATE_RUN);
}
ERun::~ERun() {
    charowner->unsetState(STATE_RUN);
    owner->clearEvents();
}
void ERun::start() {
    charowner->addMaxSpeed(dmax);
    CAnimEvent::start();
}

void ERun::end() {
    if (started) charowner->addMaxSpeed(-dmax);
    CAnimEvent::end();
}

void ERun::cancel() {
    if (started) charowner->addMaxSpeed(-dmax);
    CAnimEvent::cancel();
}

void ERun::reset() {
    t_reset=0;
}
Uint16 ERun::update(Uint16 dt) {
    Uint16 evstate=NOTHING;
    t_reset+=dt;
    if (t_reset>100) {
        evstate=EV_CANCEL;
    } else {
        evstate=CAnimEvent::update(dt);
    } 
    return evstate;
}


EAttack::EAttack(Character* chr, Uint16 length, Weapon* atweapon, Uint16 dir, Uint16 weap_range, Uint16 target_mask, Uint16 edelay, Uint32 switchstate, Mix_Chunk* esound, Animation* runanim, bool delanim):
  CAnimEvent(chr,length,edelay,switchstate|ESTATE_BUSY,esound,runanim,delanim),
  weapon(atweapon),
  direction(dir),
  range(weap_range),
  mask(target_mask) { }
void EAttack::end() {
    std::set<Character *> targets=scenario->getCharactersIn(mask,*owner->getPos(),true,range,direction);
    if (!targets.empty()) {
        (*targets.begin())->hit(direction,*weapon);
    }
    CAnimEvent::end();
}
