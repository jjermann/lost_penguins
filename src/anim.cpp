#include "common.h"
#include "input.h"
#include "gfxeng.h"
#include "objectpools.h"
#include "players_common.h"
#include "anim.h"


Animation::Animation(SDL_Surface* anim_image, Uint16 max_num, Uint16 total_time, bool an_once):
  size(max_num),
  time(total_time),
  num(0),
  tcurrent(0),
  once(an_once) {
    if (time==0 || once) running=false;
    else running=true;
    frame.image=anim_image;
    w=(Uint16)((frame.image->w)/size);
    h=frame.image->h;
    frame.pos.x=0;
    frame.pos.y=0;
    frame.pos.w=w;
    frame.pos.h=h;
}

Animation::Animation(Uint16 width, SDL_Surface* anim_image, Uint16 max_num, Uint16 total_time, bool an_once):
  size(max_num),
  time(total_time),
  num(0),
  tcurrent(0),
  once(an_once) {
    if (time==0 || once) running=false;
    else running=true;
    frame.image=anim_image;
    w=width;
    h=frame.image->h;
    frame.pos.x=0;
    frame.pos.y=0;
    frame.pos.w=w;
    frame.pos.h=h;
}

Animation::~Animation() { }

bool Animation::updateAnim(Uint16 dt) {
    if (!running) return false;
    tcurrent+=dt;
    if (tcurrent < time) {
        num=(Uint16)((tcurrent*size)/time);
        if (num>=size) num=0;
    } else if (once || time==0) {
        num=size;
        tcurrent=time;
        running=false;
    } else {
        num=0;
        tcurrent=0;
    }
    frame.pos.x=num*w;
    if (running) return true;
    else return false;
//The solution below would be better but it won't work with the 2nd constructor
//    frame.pos.x=(Uint16)((num*frame.image->w)/size);
}

const Frame& Animation::setFrame(Uint16 num) {
    frame.pos.x=num*w;
    return frame;
}

AnimHandler::AnimHandler(): 
  tstart(SDL_GetTicks()),
  dt(0) {
    tcurrent=tstart;
}

AnimHandler::~AnimHandler() {
}

Uint16 AnimHandler::resetTime() {
    dt=0;
    return (tcurrent=SDL_GetTicks());
}

void AnimHandler::runAnims() {
    dt=(SDL_GetTicks()-tcurrent);
    tcurrent=SDL_GetTicks();

    if (!paused) {
        //released keys of player
        if (player != NULL) {
            if (input->getState(INPUTR_USE)) player->in_use(-1);
            input->unsetState(INPUTR_USE);
            if (input->getState(INPUTR_ACT)) player->in_act(-1);
            input->unsetState(INPUTR_ACT);
            if (input->getState(INPUTR_RIGHT)) player->in_right(-1);
            input->unsetState(INPUTR_RIGHT);
            if (input->getState(INPUTR_LEFT)) player->in_left(-1);
            input->unsetState(INPUTR_LEFT);
            if (input->getState(INPUTR_SP1)) player->in_sp1(-1);
            input->unsetState(INPUTR_SP1);
            if (input->getState(INPUTR_SP2)) player->in_sp2(-1);
            input->unsetState(INPUTR_SP2);
            if (input->getState(INPUTR_UP)) player->in_up(-1);
            input->unsetState(INPUTR_UP);
            if (input->getState(INPUTR_DOWN)) player->in_down(-1);
            input->unsetState(INPUTR_DOWN);
            input->unsetState(INPUTR_DEL);
        }
        object_iterator obit=pool->objectspool.begin();
        while (obit!=pool->objectspool.end()) {
            //remove marked objects
            if ((*obit)->isDeleted()) {
                obit=pool->removeObject(*obit);
            } else ++obit;
        }
        obit=pool->objectspool.begin();
        while (obit!=pool->objectspool.end()) {
            (*obit)->idle(dt);
            (*obit)->updateEvents(dt);
            ++obit;
        }
        //handle current (new) player
        if ((player!=NULL) && (!(player->getState(ESTATE_BUSY)))) {
            if (input->getState(INPUT_USE)) player->in_use(dt);
            if (input->getState(INPUT_ACT)) player->in_act(dt);
            if (input->getState(INPUT_RIGHT)) player->in_right(dt);
            if (input->getState(INPUT_LEFT)) player->in_left(dt);
            if ((!(player->getState(ESTATE_RUN)))||player->getState(ESTATE_ABORT)) {
                if (input->getState(INPUT_SP1)) player->in_sp1(dt);
                if (input->getState(INPUT_SP2)) player->in_sp2(dt);
            }
            if (input->getState(INPUT_UP)) player->in_up(dt);
            if (input->getState(INPUT_DOWN)) player->in_down(dt);
        }
        //run end player effects
        character_iterator cit=pool->characterspool.begin();
        while (cit!=pool->characterspool.end()) {
            (*cit)->fall(dt);
            (*cit)->updateAnimState(!((*cit)->getState(ESTATE_ANIM)));
            ++cit;
        }
        //update the animations of all objects
        obit=pool->objectspool.begin();
        while (obit!=pool->objectspool.end()) {
            if ((*obit)->getState(ESTATE_ANIM)) { 
                bool runs=(*obit)->updateAnim(dt);
                if (!runs) (*obit)->stopEvent();
            } else if ((*obit)->isRunning()) (*obit)->updateAnim(dt);
            ++obit;
        }
    } else {
        if (input->getState(INPUT_RIGHT)) {
            input->unsetState(INPUT_RIGHT);
            player->switchItem(true);
        }
        if (input->getState(INPUT_LEFT)) {
            input->unsetState(INPUT_LEFT);
            player->switchItem(false);
        }
        if (input->getState(INPUT_DEL)) {
            input->unsetState(INPUT_DEL);
            player->dropItem();
            gfxeng->renderScene(true);
        }
    }
}
