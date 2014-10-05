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


Erik::Erik(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Player(xcord,ycord,param),
  jump(V_JUMP),
  jump2(V_JUMP2) {
    weapon=Weapon(-1,W_PRESSURE,WS_PRESSURE);

    /* Parameters */
    if (hasParam(parameters,"anim_erik_jump_left"))          anim_erik_jump_left=loadAnimation(getParameters(parameters["anim_erik_jump_left"],':'));
    if (hasParam(parameters,"anim_erik_jump_right"))         anim_erik_jump_right=loadAnimation(getParameters(parameters["anim_erik_jump_right"],':'));
    if (hasParam(parameters,"anim_erik_jump2_left"))         anim_erik_jump2_left=loadAnimation(getParameters(parameters["anim_erik_jump2_left"],':'));
    if (hasParam(parameters,"anim_erik_jump2_right"))        anim_erik_jump2_right=loadAnimation(getParameters(parameters["anim_erik_jump2_right"],':'));
    if (hasParam(parameters,"anim_erik_start_run_left"))     anim_erik_start_run_left=loadAnimation(getParameters(parameters["anim_erik_start_run_left"],':'));
    if (hasParam(parameters,"anim_erik_start_run_right"))    anim_erik_start_run_right=loadAnimation(getParameters(parameters["anim_erik_start_run_right"],':'));
    if (hasParam(parameters,"anim_erik_run_left"))           anim_erik_run_left=loadAnimation(getParameters(parameters["anim_erik_run_left"],':'));
    if (hasParam(parameters,"anim_erik_run_right"))          anim_erik_run_right=loadAnimation(getParameters(parameters["anim_erik_run_right"],':'));
    if (hasParam(parameters,"anim_erik_swim_left"))          anim_erik_swim_left=loadAnimation(getParameters(parameters["anim_erik_swim_left"],':'));
    if (hasParam(parameters,"anim_erik_swim_right"))         anim_erik_swim_right=loadAnimation(getParameters(parameters["anim_erik_swim_right"],':'));
    if (hasParam(parameters,"anim_erik_swim_up_left"))       anim_erik_swim_up_left=loadAnimation(getParameters(parameters["anim_erik_swim_up_left"],':'));
    if (hasParam(parameters,"anim_erik_swim_up_right"))      anim_erik_swim_up_right=loadAnimation(getParameters(parameters["anim_erik_swim_up_right"],':'));
    if (hasParam(parameters,"anim_erik_hit_water_left"))     anim_erik_hit_water_left=loadAnimation(getParameters(parameters["anim_erik_hit_water_left"],':'));
    if (hasParam(parameters,"anim_erik_hit_water_right"))    anim_erik_hit_water_right=loadAnimation(getParameters(parameters["anim_erik_hit_water_right"],':'));

    if (!hasParam(parameters,"audio_hit")) au_hit=scenario->sndcache->loadWAV("erikhit.wav");

    if (hasParam(parameters,"audio_jump")) au_jump=scenario->sndcache->loadWAV(parameters["audio_jump"]);
      else au_jump=scenario->sndcache->loadWAV("rboots.wav");
    if (hasParam(parameters,"audio_run")) au_jump=scenario->sndcache->loadWAV(parameters["audio_run"]);
      else au_run=NULL;
}

Erik::~Erik() { }

void Erik::updateAnimState() {
    if (state&STATE_WATER) {
        if (state&STATE_LEFT) {
            if (state&STATE_MLEFT) {
                setAnim(anim_erik_swim_left);
            } else if (state&STATE_MUP) {
                setAnim(anim_erik_swim_up_left);
            //} else if (state&STATE_MDOWN) {
            //    setAnim(anim_erik_swim_down_left);
            } else {
                setAnim(anim_erik_swim_left);
            }
        } else {
            if (state&STATE_MRIGHT) {
                setAnim(anim_erik_swim_right);
            } else if (state&STATE_MUP) {
                setAnim(anim_erik_swim_up_right);
            //} else if (state&STATE_MDOWN) {
            //    setAnim(anim_erik_swim_down_right);
            } else {
                setAnim(anim_erik_swim_right);
            }
        }
    } else {
        Player::updateAnimState();
    }
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
            setEvent(new ERun(this,10000,maxspeedx,500,ESTATE_ABORT,au_run,(state&STATE_LEFT) ? anim_erik_run_left : anim_erik_run_right));
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
        setEvent(new CAnimEvent(this,DE_JUMP,0,ESTATE_ABORT,au_jump,(state&STATE_LEFT) ? anim_erik_jump2_left : anim_erik_jump2_right));
    } else {
        setState(STATE_ACT_1);
        addSpeed(jump);
        setEvent(new CAnimEvent(this,DE_JUMP,0,ESTATE_ABORT,NULL,(state&STATE_LEFT) ? anim_erik_jump_left : anim_erik_jump_right));
    }
}

void Erik::in_sp2() {
    //TODO: check STATE_WATER
}

void Erik::in_up(Uint16 dt) {
    if (state&STATE_WATER) {
        speed=-maxspeedy;
    }
    Player::in_up(dt);
}

void Erik::in_down(Uint16 dt) {
    if (state&STATE_WATER) {
        speed=maxspeedy;
    }
    Player::in_down(dt);
}

void Erik::in_left(Uint16 dt) {
    if (state&STATE_RUN) {
        if (state&STATE_LEFT) event->reset();
        //changed directions, TODO: play decelerate animation
        else cancelEvent();
    } else if (input->keyState(KEY_SP2)) {
        run_right=false;
        setEvent(new ERun(this,10000,maxspeedx,500,ESTATE_ABORT,au_run,anim_erik_run_left));
    }
    Player::in_left(dt);
}

void Erik::in_right(Uint16 dt) {
    if (state&STATE_RUN) {
        if (!(state&STATE_LEFT)) event->reset();
        //changed directions, TODO: play decelerate animation
        else cancelEvent();
    } else if (input->keyState(KEY_SP2)) {
        run_right=true;
        setEvent(new ERun(this,10000,maxspeedx,500,ESTATE_ABORT,au_run,anim_erik_run_right));
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
