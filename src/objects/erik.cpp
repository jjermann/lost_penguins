#include "common.h"
#include "events.h"
#include "animation.h"
#include "input.h"
#include "scenario.h"
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
    im_left=loadAnimation(scenario->imgcache->loadImage("erik1_left.bmp"));
    im_right=loadAnimation(scenario->imgcache->loadImage(1,"erik1_right.bmp"));
    im_run_right=im_right;
    im_run_left=im_left;
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),T_IRR,1,ATYPE_ONCE_END);
    im_land_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),T_IRR,1,ATYPE_ONCE_END);
    au_jump=scenario->sndcache->loadWAV("rboots.wav");
    au_hit=scenario->sndcache->loadWAV("erikhit.wav");
    au_run=NULL;
}
Erik::~Erik() {
    delete im_left;
    delete im_right;
    delete im_land_left;
    delete im_land_right;
}

void Erik::idle(Uint16 dt) {
    Player::idle(dt);
    //TODO: this is an ugly hack...
    if (!input->keyState(KEY_SP2)) {
        dense_types&=~OTYPE_MONSTER;
        if (state&STATE_RUN) cancelEvent();
    } else if (state&STATE_RUN) {
        dense_types|=OTYPE_MONSTER;
        if (state&STATE_FALL) cancelEvent();
        if ((!state&STATE_MRIGHT) && run_right) cancelEvent();
        if ((!state&STATE_MLEFT) && (!run_right)) cancelEvent();
    } else {
        //TODO: check STATE_WATER
        if (state&(STATE_MLEFT|STATE_MRIGHT)) {
            if (state&STATE_MRIGHT) run_right=true;
            else run_right=false;
            setEvent(new ERun(this,10000,maxspeedx,500,ESTATE_ABORT,au_run));
        }
    }
}

void Erik::in_sp1() {
    //TODO: check STATE_WATER
    if (state&STATE_FALL) setState(STATE_ACT_1);

    if (state&STATE_ACT_2) {
    } else if (state&STATE_ACT_1) {
        setState(STATE_ACT_2);
        addSpeed(jump2);
        setEvent(new CAnimEvent(this,DE_JUMP,0,0,au_jump));
    } else {
        setState(STATE_ACT_1);
        addSpeed(jump);
        setEvent(new CAnimEvent(this,DE_JUMP));
    }
}

void Erik::in_sp2() {
    //TODO: check STATE_WATER
    if (state&(STATE_MLEFT|STATE_MRIGHT)) {
        if (state&STATE_MRIGHT) run_right=true;
        else run_right=false;
        setEvent(new ERun(this,10000,maxspeedx,500,ESTATE_ABORT,au_run));
    }
}

void Erik::in_left(Uint16 dt) {
    if (state&STATE_RUN) {
        if (state&STATE_LEFT) event->reset();
        //changed directions, TODO: play decelerate animation
        else cancelEvent();
    }
    Player::in_left(dt);
}

void Erik::in_right(Uint16 dt) {
    if (state&STATE_RUN) {
        if (!(state&STATE_LEFT)) event->reset();
        //changed directions, TODO: play decelerate animation
        else cancelEvent();
    }
    Player::in_right(dt);
}

void Erik::crash(Uint16 dir) {
    if ((state&(STATE_PRESS_LR|STATE_ACT_2)) && (!(dir&DIR_DOWN))) {
        std::set<Character *> targets=scenario->getCharactersIn(OTYPE_MONSTER,getCenter(),true,pos.w+1,dir);
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
