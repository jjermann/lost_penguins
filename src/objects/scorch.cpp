#include "common.h"
#include "events.h"
#include "anim.h"
#include "input.h"
#include "imgcache.h"
#include "sndcache.h"
#include "scorch.h"


Scorch::Scorch(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname),
  left_wings(SCORCH_MAX_WINGS),
  wing(V_FLY) {
    im_left=new Animation(imgcache->loadImage("baleog_right.bmp"));
    im_right=im_left;
    im_fall_left=im_left;
    im_fall_right=im_left;
    im_krit_left=im_left;
    im_krit_right=im_left;
    im_land_left=new Animation(imgcache->loadImage("olaf_land.bmp"),1,T_IRR,true);
    im_land_right=im_land_left;
    au_swing=sndcache->loadWAV("flapwngs.wav");
    au_tired=sndcache->loadWAV("flwings.wav");
    au_hit=sndcache->loadWAV("draghit.wav");
}
Scorch::~Scorch() {
    delete im_left;
    delete im_land_left;
}

void Scorch::fall(Uint16 dt) {
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

void Scorch::in_sp1(Sint16 dt) {
    if (dt < 0) {
        unsetState(STATE_ACT_1);
        unsetState(STATE_GLIDE);
        input->unsetState(INPUT_SP1);
        return;
    }
    setState(STATE_FALL);
    //if not exhausted -> glide
    if (!(state&STATE_ACT_2)) {
        setState(STATE_GLIDE);
    }
    //Can't fly anymore
    if ((state&STATE_ACT_1)||(state&STATE_ACT_2)) {
    } else if (left_wings<=0) {
        setState(STATE_ACT_1);
        setState(STATE_ACT_2);
        unsetState(STATE_GLIDE);
        setEvent(new ESpeed(this,DE_WING,V_FLY,0,0,0,au_tired));
    //Use Wings
    } else {
        setState(STATE_ACT_1);
        left_wings--;
        setEvent(new ESpeed(this,DE_WING,V_FLY,0,0,0,au_swing));
    }
}

void Scorch::clearStates(bool reset) {
    Player::clearStates(reset);
    if (reset) {
        unsetState(STATE_GLIDE);
        input->unsetState(INPUT_SP1);
        left_wings=SCORCH_MAX_WINGS;
    }
}
