#include "common.h"
#include "animation.h"

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

