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

ELand::ELand(Character* chr, Uint16 length, Mix_Chunk* esound):
  CharacterEvent(chr,length,0,(STATE_IRR|ESTATE_BUSY),esound) { }
void ELand::start() {
    charowner->addHealth(-1);
    cout << "Autsch!\n";
    CharacterEvent::start();
}

ESpeed::ESpeed(Character* chr, Uint16 length, Sint16 ajspeed, Sint16 ahspeed, Sint16 avspeed, Uint16 edelay, Uint32 switchstate, Mix_Chunk* esound):
  CharacterEvent(chr,length,edelay,switchstate,esound), jspeed(ajspeed),hspeed(ahspeed),vspeed(avspeed) { }
void ESpeed::start() {
    if (jspeed<0) charowner->setState(STATE_FALL);
    charowner->addSpeed(jspeed);
    CharacterEvent::start();
}
Uint16 ESpeed::update(Uint16 dt) {
    Uint16 evstate=CharacterEvent::update(dt);
    if (evstate&(EV_RUN|EV_START)) {
        charowner->addHSpeed(hspeed);
        charowner->addVSpeed(vspeed);
    }
    return evstate;
}
