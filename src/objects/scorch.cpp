#include "common.h"
#include "events.h"
#include "animation.h"
#include "input.h"
#include "imgcache.h"
#include "scenario.h"
#include "sndcache.h"
#include "scorch.h"


Scorch::Scorch(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname),
  left_wings(SCORCH_MAX_WINGS),
  wing(V_FLY) {
    anim_left=loadAnimation(scenario->imgcache->loadImage(1,"baleog1_left.bmp"));
    anim_right=loadAnimation(scenario->imgcache->loadImage(1,"baleog1_right.bmp"));
    anim_walk_left=loadAnimation(scenario->imgcache->loadImage(8,"baleog1-run_left.png"),8);
    anim_walk_right=loadAnimation(scenario->imgcache->loadImage(8,"baleog1-run_right.png"),8);
    anim_crash_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),1,BP_MD,ATYPE_ONCE_END,calcFPS(1,T_IRR));
    anim_crash_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),1,BP_MD,ATYPE_ONCE_END,calcFPS(1,T_IRR));
    au_swing=scenario->sndcache->loadWAV("flapwngs.wav");
    au_tired=scenario->sndcache->loadWAV("flwings.wav");
    au_hit=scenario->sndcache->loadWAV("draghit.wav");
}
Scorch::~Scorch() {
}

void Scorch::idle(Uint16 dt) {
    Player::idle(dt);
    if (!input->keyState(KEY_SP1)) {
        unsetState(STATE_GLIDE);
    } else if (!(state&STATE_ACT_2)) {
        setState(STATE_GLIDE);
    }
}

void Scorch::fall(Uint16 dt) {
    if (!getState(STATE_MRIGHT|STATE_MLEFT)) {
        if (!getState(STATE_FALL)) hspeed=boost(hspeed,-dt*HSPEED_MULT/100);
        else hspeed=boost(hspeed,-dt*HSPEED_MULT/200);
    }
    Dgrav+=dt;
    if (Dgrav>T_GRAV_EFFECT) {
        if (state&STATE_FALL) {
            addSpeed(Sint16(gravity*Dgrav/1000));
            if ((state&STATE_GLIDE) && (speed > V_GLIDE)) speed=V_GLIDE;;
        }
        Hit hit=move(Dgrav);
        if ((hit.enter&DIR_DOWN) && (state&STATE_FALL)) crash(DIR_DOWN);
        Dgrav=0;
    }
}

void Scorch::in_sp1() {
    setState(STATE_FALL);
    //Can't fly anymore
    if (state&STATE_ACT_2) {
    } else if (left_wings<=0) {
        setState(STATE_ACT_2);
        unsetState(STATE_GLIDE);
        addSpeed(V_FLY);
        setEvent(new CAnimEvent(this,DE_WING,0,0,au_tired));
    //Use Wings
    } else {
        left_wings--;
        addSpeed(V_FLY);
        setEvent(new CAnimEvent(this,DE_WING,0,0,au_swing));
    }
}

void Scorch::clearStates(bool reset) {
    Player::clearStates(reset);
    if (reset) {
        unsetState(STATE_GLIDE);
        left_wings=SCORCH_MAX_WINGS;
    }
}
