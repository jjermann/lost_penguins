#include "common.h"
#include "events.h"
#include "anim.h"
#include "input.h"
#include "imgcache.h"
#include "sndcache.h"
#include "fang.h"


Fang::Fang(string imagename, Uint16 xcord, Uint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname) {
    im_left=new Animation(imgcache->loadImage("olaf_left.bmp"));
    im_right=new Animation(imgcache->loadImage("olaf_right.bmp"));
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=new Animation(imgcache->loadImage("olaf_land.bmp"),1,T_IRR,true);
    im_land_right=im_land_left;
    au_jump=NULL;
    jump=V_JUMP;
}
Fang::~Fang() {
    delete im_left;
    delete im_right;
    delete im_land_left;
}

void Fang::in_left(Sint16 dt) {
    Player::in_left(dt);
    if (dt >= 0) unsetState(STATE_CLIMB_R);
}
void Fang::in_right(Sint16 dt) {
    Player::in_right(dt);
    if (dt >= 0) unsetState(STATE_CLIMB_L);
}

void Fang::fall(Uint16 dt) {
    Dgrav+=dt;
    if (Dgrav>T_GRAV_EFFECT) {
        if (state&STATE_FALL) {
            addSpeed(Sint16(gravity*Dgrav/1000));
            if ((state&STATE_CLIMB) && (speed>V_CLIMB)) speed=V_CLIMB;
        }
        Hit hit=move(Dgrav);
        if (state&STATE_FALL) {
            //TODO: clean this up
            //Did we left the wall?
            if (!(hit.touch&DIR_LEFT)) unsetState(STATE_CLIMB_L);
            if (!(hit.touch&DIR_RIGHT)) unsetState(STATE_CLIMB_R);
        }
        if ((hit.enter&DIR_DOWN || hit.touch&DIR_DOWN) && (state&STATE_FALL)) crash(DIR_DOWN);
        Dgrav=0;
    }
}

//Fang1: Jump
//Act1: jumped
void Fang::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_SP1);

    if (state&STATE_CLIMB_L) {
        unsetState(STATE_LEFT);
        unsetState(STATE_CLIMB_L);
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,T_JUMPOFF,V_JUMPOFF,Sint16(maxspeedx*1.5),0,0,au_jump));
    } else if (state&STATE_CLIMB_R) {
        setState(STATE_LEFT);
        unsetState(STATE_CLIMB_R);
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,T_JUMPOFF,V_JUMPOFF,Sint16(-maxspeedx*1.5),0,0,au_jump));
    } else if ((!(state&STATE_FALL)) && (!(state&STATE_ACT_1))){
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,DE_JUMP,V_JUMP,0,0,0,au_jump));
    }
}

void Fang::crash(Uint16 dir) {
    Player::crash(dir);
    switch (dir) {
        case DIR_LEFT: {
            setState(STATE_CLIMB_L);
            break;
        }
        case DIR_RIGHT: {
            setState(STATE_CLIMB_R);
            break;
        }
        default: { }
    }
}

void Fang::clearStates(bool reset) {
    Player::clearStates(reset);
    if (reset) {
        unsetState(STATE_CLIMB_R);
        unsetState(STATE_CLIMB_L);
    }
}
