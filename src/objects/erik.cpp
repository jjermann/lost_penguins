#include "common.h"
#include "events.h"
#include "anim.h"
#include "input.h"
#include "map.h"
#include "weapons.h"
#include "imgcache.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "erik.h"


Erik::Erik(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname),
  jump(V_JUMP),
  jump2(V_JUMP2) {
    weapon=Weapon(-1,W_PRESSURE,WS_PRESSURE);
    im_left=new Animation(imgcache->loadImage("erik_left.bmp"));
    im_right=new Animation(imgcache->loadImage("erik_right.bmp"));
    im_run_right=im_right;
    im_run_left=im_left;
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=new Animation(imgcache->loadImage("olaf_land_left.bmp"),1,T_IRR,true);
    im_land_right=new Animation(imgcache->loadImage("olaf_land_right.bmp"),1,T_IRR,true);
    au_jump=sndcache->loadWAV("rboots.wav");
    au_hit=sndcache->loadWAV("erikhit.wav");
    au_run=NULL;
}
Erik::~Erik() {
    delete im_left;
    delete im_right;
    delete im_land_left;
    delete im_land_right;
}

void Erik::in_sp1(Sint16 dt) {
    //TODO: check STATE_WATER
    if (dt < 0) return;
    input->unsetState(INPUT_SP1);

    if (state&STATE_FALL) setState(STATE_ACT_1);

    if (state&STATE_ACT_2) {
    } else if (state&STATE_ACT_1) {
        setState(STATE_ACT_2);
        setEvent(new ESpeed(this,DE_JUMP,jump2,0,0,0,au_jump));
    } else {
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,DE_JUMP,jump));
    }
}

void Erik::in_sp2(Sint16 dt) {
    //TODO: check STATE_WATER
    if (dt < 0) {
        input->unsetState(INPUT_SP2);
        dense_types&=~OTYPE_MONSTER;
        if (state&STATE_RUN) cancelEvent();
        return;
    }
    dense_types|=OTYPE_MONSTER;
    if (state&STATE_RUN) {
        if (state&STATE_FALL) cancelEvent();
    } else if (state&STATE_MLEFT) {
        sfxeng->playWAV(au_run);
        setEvent(new ERun(this,10000,Sint16(-1/2*maxspeedx),-maxspeedx,500,ESTATE_ABORT,au_run));
    } else if (state&STATE_MRIGHT) {
        sfxeng->playWAV(au_run);
        setEvent(new ERun(this,10000,Sint16(1/2*maxspeedx),maxspeedx,500,ESTATE_ABORT,au_run));
    }
}

void Erik::in_left(Sint16 dt) {
    if (dt < 0) {
        //TODO: play decelerate animation (setEvent instead)
        if (state&STATE_RUN) cancelEvent();
    } else if (state&STATE_RUN) {
        if (state&STATE_LEFT) event->reset();
        //changed directions, TODO: play decelerate animation
        else cancelEvent();
    }
    Player::in_left(dt);
}

void Erik::in_right(Sint16 dt) {
    if (dt < 0) {
        //TODO: play decelerate animation (setEvent instead)
        if (state&STATE_RUN) cancelEvent();
    } else if (state&STATE_RUN) {
        if (!(state&STATE_LEFT)) event->reset();
        //changed directions, TODO: play decelerate animation
        else cancelEvent();
    }
    Player::in_right(dt);
}

void Erik::crash(Uint16 dir) {
    if ((state&(STATE_PRESS_LR|STATE_ACT_2)) && (!(dir&DIR_DOWN))) {
        std::set<Character *> targets=curmap->getCharactersIn(OTYPE_MONSTER,getCenter(),true,pos.w+1,dir);
        ///\bug This might get the wrong target
        if (!targets.empty()) (*targets.begin())->hit(dir,weapon);
    }
    Player::crash(dir);
}

Uint16 Erik::hit(Uint16 dir, Weapon& weap) {
    if (weap.getType()==W_WATER) {
        cancelEvent();
        return health;
    } else return Player::hit(dir,weap);
}
