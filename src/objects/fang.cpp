#include "common.h"
#include "events.h"
#include "animation.h"
#include "input.h"
#include "imgcache.h"
#include "sndcache.h"
#include "scenario.h"
#include "fang.h"


Fang::Fang(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname),
  jump(V_JUMP) {
    weapon=Weapon(-1,W_STRIKE);
    im_left=loadAnimation(scenario->imgcache->loadImage(4,"Fang_Breath_left.png"),1000,4);
    im_right=loadAnimation(scenario->imgcache->loadImage(4,"Fang_Breath_right.png"),1000,4);
    im_run_left=loadAnimation(scenario->imgcache->loadImage(8,"Fang_walk_left.png"),1000,8);
    im_run_right=loadAnimation(scenario->imgcache->loadImage(8,"Fang_walk_right.png"),1000,8);
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),T_IRR,1,ATYPE_ONCE_END);
    im_land_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),T_IRR,1,ATYPE_ONCE_END);
    im_claw_left=loadAnimation(scenario->imgcache->loadImage(8,"Fang_Clawslash_left.png"),1000,8,ATYPE_ONCE_END);
    im_claw_right=loadAnimation(scenario->imgcache->loadImage(8,"Fang_Clawslash_right.png"),1000,8,ATYPE_ONCE_END);
    au_hit=scenario->sndcache->loadWAV("wolfhit.wav");
    au_claw=scenario->sndcache->loadWAV("wolfjmp1.wav");
    au_jump=scenario->sndcache->loadWAV("fangjump.wav");
}
Fang::~Fang() {
    delete im_left;
    delete im_right;
    delete im_run_left;
    delete im_run_right;
    delete im_land_left;
    delete im_land_right;
    delete im_claw_left;
    delete im_claw_right;
}

void Fang::in_left(Uint16 dt) {
    Player::in_left(dt);
    unsetState(STATE_CLIMB_R);
}
void Fang::in_right(Uint16 dt) {
    Player::in_right(dt);
    unsetState(STATE_CLIMB_L);
}

void Fang::fall(Uint16 dt) {
    if (!getState(STATE_MRIGHT|STATE_MLEFT)) {
        if (!getState(STATE_FALL)) hspeed=boost(hspeed,-dt*HSPEED_MULT/100);
        else hspeed=boost(hspeed,-dt*HSPEED_MULT/200);
    }
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
        if ((hit.enter&DIR_DOWN) && (state&STATE_FALL)) crash(DIR_DOWN);
        Dgrav=0;
    }
}

void Fang::in_sp1() {
    if (state&STATE_CLIMB_L) {
        unsetState(STATE_LEFT);
        unsetState(STATE_CLIMB_L);
        setState(STATE_ACT_1);
        addSpeed(V_JUMPOFF);
        addHSpeed(maxspeedx);
        setEvent(new CAnimEvent(this,T_JUMPOFF,0,0,au_jump));
    } else if (state&STATE_CLIMB_R) {
        setState(STATE_LEFT);
        unsetState(STATE_CLIMB_R);
        setState(STATE_ACT_1);
        addSpeed(V_JUMPOFF);
        addHSpeed(-maxspeedx);
        setEvent(new CAnimEvent(this,T_JUMPOFF,0,0,au_jump));
    } else if ((!(state&STATE_FALL)) && (!(state&STATE_ACT_1))){
        setState(STATE_ACT_1);
        addSpeed(V_JUMP);
        setEvent(new CAnimEvent(this,DE_JUMP,0,0,au_jump));
    }
}

void Fang::in_sp2() {
    setEvent(new EAttack(this,10,&weapon,(state&STATE_LEFT) ? DIR_LEFT : DIR_RIGHT,10,enemy_types,0,0,au_claw,(state&STATE_LEFT) ? im_claw_left : im_claw_right));
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
