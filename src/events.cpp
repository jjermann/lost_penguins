#include "lost_vikings.h"
#include "events.h"
#include "objects.h"
#include "weapons.h"

using namespace std;


//Common Events (basically sets the events idle for length)
Event::Event(Object* obj, Uint16 length, Uint16 edelay):
  owner(obj),duration(length),delay(edelay),tleft(delay),started(false) { }
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
    started=true;
}
void Event::end() {
    delete this;
}
void Event::cancel() {
    delete this;
}


//CharacterEvent
CharacterEvent::CharacterEvent(Character* chr, Uint16 length, Uint16 edelay, Uint32 switchstate, Mix_Chunk* esound):
  Event(chr,length,edelay), charowner(chr), state(switchstate), sound(esound) {
    state|=ESTATE_RUN;
}
void CharacterEvent::start() {
    if (state) charowner->switchState(state);
    sfxeng->playWAV(sound);
    Event::start();
}
void CharacterEvent::end() {
    if (started && state) charowner->switchState(state);
    Event::end();
}
void CharacterEvent::cancel() {
    if (started && state) charowner->switchState(state);
    Event::cancel();
}


ESpeed::ESpeed(Character* chr, Uint16 length, Sint16 avspeed, Sint16 ahspeed, Uint16 edelay, Uint32 switchstate, Mix_Chunk* esound):
  CharacterEvent(chr,length,edelay,switchstate,esound),vspeed(avspeed),hspeed(ahspeed) { }
void ESpeed::start() {
    charowner->addSpeed(vspeed);
    charowner->addHSpeed(hspeed);
    CharacterEvent::start();
}
void ESpeed::end() {
    if (started) charowner->addHSpeed(-hspeed);
    CharacterEvent::end();
}
void ESpeed::cancel() {
    if (started) charowner->addHSpeed(-hspeed);
    CharacterEvent::cancel();
}


ERun::ERun(Character* chr, Uint16 length, Sint16 inispeed, Sint16 ahspeed, Uint16 edelay, Uint32 switchstate, Mix_Chunk* esound):
  ESpeed(chr,length,0,ahspeed,edelay,switchstate,esound),ispeed(inispeed),t_reset(0) {
    charowner->setState(STATE_RUN);
    charowner->addHSpeed(ispeed);
}
ERun::~ERun() {
    charowner->unsetState(STATE_RUN);
    charowner->addHSpeed(-ispeed);
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
        evstate=CharacterEvent::update(dt);
    } 
    return evstate;
}


EAnim::EAnim(Character* chr, Animation* runanim, bool delanim, Uint16 edelay, Uint32 switchstate, Mix_Chunk* esound):
  CharacterEvent(chr,30000,edelay,(switchstate|ESTATE_BUSY|ESTATE_ANIM),esound),anim(runanim),del(delanim) {
    if (anim==NULL) cancel();
}
void EAnim::start() {
    CharacterEvent::start();
    charowner->setAnim(anim);
}
void EAnim::end() {
    //hack!!!
    if (del) delete anim;
    if (started) charowner->updateAnimState();
    CharacterEvent::end();
}
void EAnim::cancel() {
    end();
}
