#include "lost_vikings.h"
#include "events.h"

using namespace std;


//Common Events (basically sets the events idle for length)
Event::Event(Object* obj, Uint16 length, Uint16 edelay):
  owner(obj),duration(length),delay(edelay),tleft(delay),started(false) { }
Event::~Event() {
    owner->clearEvents();
}
Uint16 Event::update(Uint16 dt) {
    tleft-=dt;
    if (tleft<=0) {
        //end
        if (started) {
            tleft=0;
            return EV_END;
        //start
        } else {
            tleft=duration;
            return EV_START;
        }
    //running
    } else if (started) {
        return EV_RUN;
    //delayed
    } else {
        return EV_DELAY;
    }
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
  Event(chr,length,edelay), charowner(chr), state(switchstate), sound(esound) { }
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

//Jump
EJump::EJump(Character* chr, Uint16 length, Speed aspeed, Uint16 edelay, Uint32 switchstate, Mix_Chunk* au_jump):
  CharacterEvent(chr,length,edelay,switchstate,au_jump), speed(aspeed) { }
void EJump::start() {
    charowner->setState(STATE_FALL);
    charowner->addSpeed(speed);
    CharacterEvent::start();
}
