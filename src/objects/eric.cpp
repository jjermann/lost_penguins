#include "common.h"
#include "events.h"
#include "anim.h"
#include "input.h"
#include "map.h"
#include "weapons.h"
#include "imgcache.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "eric.h"


Eric::Eric(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname),
  jump(V_JUMP),
  jump2(V_JUMP2) {
    weapon=Weapon(-1,W_PRESSURE,WS_PRESSURE);
    im_left=new Animation(imgcache->loadImage("eric_left.bmp"));
    im_right=new Animation(60,imgcache->loadImage("kuru.bmp"),12,1000);
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=new Animation(imgcache->loadImage("olaf_land.bmp"),1,T_IRR,true);
    im_land_right=im_land_left;
    au_jump=sndcache->loadWAV("rboots.wav");
    au_hit=sndcache->loadWAV("erikhit.wav");
    au_run=NULL;
}
Eric::~Eric() {
    delete im_left;
    delete im_right;
    delete im_land_left;
}

void Eric::in_sp1(Sint16 dt) {
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

void Eric::in_sp2(Sint16 dt) {
    //TODO: check STATE_WATER
    if (dt < 0) {
        input->unsetState(INPUT_SP2);
        if (state&STATE_RUN) cancelEvent();
        return;
    }
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

void Eric::in_left(Sint16 dt) {
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

void Eric::in_right(Sint16 dt) {
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

void Eric::crash(Uint16 dir) {
    if ((state&(STATE_PRESS_LR|STATE_ACT_2)) && (!(dir&DIR_DOWN))) {
        std::set<Character *> targets=curmap->getCharactersIn(OTYPE_MONSTER,getCenter(),true,pos.w+1,dir);
        ///\bug This might get the wrong target
        if (!targets.empty()) (*targets.begin())->hit(dir,weapon);
    }
    Player::crash(dir);
}

Uint16 Eric::hit(Uint16 dir, Weapon& weap) {
    if (weap.getType()==W_WATER) {
        cancelEvent();
        return health;
    } else return Player::hit(dir,weap);
}
