#include "common.h"
#include "input.h"
#include "gfxeng.h"
#include "objectpools.h"
#include "vikings_common.h"
#include "anim.h"

using namespace std;


//Initialize an animation: animation image, number of frames,
//time intervall for the animation, if (total_time==0) it is considered a frames series
Animation::Animation(SDL_Surface* anim_image, Uint16 max_num, Uint16 total_time, bool an_once):
  size(max_num),time(total_time),num(0),tcurrent(0),once(an_once) {
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

//Initialize an animation: animation image, width of a frame, number of frames,
//time intervall for the animation, if (total_time==0) it is considered a frames series
Animation::Animation(Uint16 width, SDL_Surface* anim_image, Uint16 max_num, Uint16 total_time, bool an_once):
  size(max_num),time(total_time),num(0),tcurrent(0),once(an_once) {
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

//TODO: this should be more advanced
//updates a running animation, returns true if it's running
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

AnimHandler::AnimHandler() {
    //Initialization
    tstart=SDL_GetTicks();
    tcurrent=tstart;
    dt=0;
}

AnimHandler::~AnimHandler() {
}

//ACT:  Resets the current time
//POST: new current time
Uint16 AnimHandler::resetTime() {
    dt=0;
    return (tcurrent=SDL_GetTicks());
}

void AnimHandler::runAnims() {
    dt=(SDL_GetTicks()-tcurrent);
    tcurrent=SDL_GetTicks();

    if (!paused) {
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
        //handle current viking
        if ((viking!=NULL) && (!(viking->getState(ESTATE_BUSY)))) {
            if (input->getState(INPUT_USE)) viking->in_use(dt);
            if (input->getState(INPUT_ACT)) viking->in_act(dt);
            if (input->getState(INPUT_RIGHT)) viking->in_right(dt);
            if (input->getState(INPUT_LEFT)) viking->in_left(dt);
            if ((!(viking->getState(ESTATE_RUN)))||viking->getState(ESTATE_ABORT)) {
                if (input->getState(INPUT_SP1)) viking->in_sp1(dt);
                if (input->getState(INPUT_SP2)) viking->in_sp2(dt);
            }
            if (input->getState(INPUT_UP)) viking->in_up(dt);
            if (input->getState(INPUT_DOWN)) viking->in_down(dt);
        }
        //run end viking effects
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
            viking->switchItem(true);
        }
        if (input->getState(INPUT_LEFT)) {
            input->unsetState(INPUT_LEFT);
            viking->switchItem(false);
        }
        if (input->getState(INPUT_DEL)) {
            input->unsetState(INPUT_DEL);
            if (viking->getState(STATE_LEFT)) viking->dropItem(false);
            else viking->dropItem(true);
            gfxeng->renderScene(true);
        }
    }
}
