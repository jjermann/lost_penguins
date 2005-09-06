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
/*
    anim_left=loadAnimation(scenario->imgcache->loadImage("erik1_left.bmp"));
    anim_right=loadAnimation(scenario->imgcache->loadImage(1,"erik1_right.bmp"));
    anim_land_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),1,BP_LD,ATYPE_ONCE,calcFPS(1,T_IRR));
    anim_land_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),1,BP_RD,ATYPE_ONCE,calcFPS(1,T_IRR));
    anim_crash_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),1,BP_LD,ATYPE_ONCE,calcFPS(1,T_IRR));
    anim_crash_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),1,BP_RD,ATYPE_ONCE,calcFPS(1,T_IRR));
*/
    anim_left=loadAnimation("erik_idle_left",config.lvlscale,BP_RD);
    anim_right=loadAnimation("erik_idle_right",config.lvlscale,BP_LD);
    anim_rock_left=loadAnimation("erik_rock_left",config.lvlscale,BP_RD);
    anim_rock_right=loadAnimation("erik_rock_right",config.lvlscale,BP_LD);
    anim_walk_left=loadAnimation("erik_walk_left",config.lvlscale,BP_RD);
    anim_walk_right=loadAnimation("erik_walk_right",config.lvlscale,BP_LD);
    anim_push_left=loadAnimation("erik_push_left",config.lvlscale,BP_RD);
    anim_push_right=loadAnimation("erik_push_right",config.lvlscale,BP_LD);
    anim_fall_middle=loadAnimation("erik_fall_middle",config.lvlscale);
    anim_fall_left=loadAnimation("erik_fall_left",config.lvlscale,BP_RD);
    anim_fall_right=loadAnimation("erik_fall_right",config.lvlscale,BP_LD);
    anim_fall_fast_left=loadAnimation("erik_fall_fast_left",config.lvlscale,BP_RD);
    anim_fall_fast_right=loadAnimation("erik_fall_fast_right",config.lvlscale,BP_LD);
    anim_crash_left=loadAnimation("erik_crash_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_crash_right=loadAnimation("erik_crash_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_rope_left=loadAnimation("erik_rope_left",config.lvlscale,BP_RD);
    anim_rope_right=loadAnimation("erik_rope_right",config.lvlscale,BP_LD);
    anim_teleport_left=loadAnimation("erik_teleport_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_teleport_right=loadAnimation("erik_teleport_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_die_crash_left=loadAnimation("erik_die_crash_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_die_crash_right=loadAnimation("erik_die_crash_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_die_burn_left=loadAnimation("erik_die_burn_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_die_burn_right=loadAnimation("erik_die_burn_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_die_bones_left=loadAnimation("erik_die_bones_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_die_bones_right=loadAnimation("erik_die_bones_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_die_elec_left=loadAnimation("erik_die_elec_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_die_elec_right=loadAnimation("erik_die_elec_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_die_spike_left=loadAnimation("erik_die_spike_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_die_spike_right=loadAnimation("erik_die_spike_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_bar=loadAnimation("bar_erik",config.lvlscale,BP_MD,ATYPE_ONCE);
    /* anim_die_water is missing as eric doesn't die under water, anim_climb is missing as well */

    anim_erik_jump_left=loadAnimation("erik_jump_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_erik_jump_right=loadAnimation("erik_jump_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_erik_jump2_left=loadAnimation("erik_jump2_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_erik_jump2_right=loadAnimation("erik_jump2_right",config.lvlscale,BP_LD,ATYPE_ONCE);
    anim_erik_start_run_left=loadAnimation("erik_start_run_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_erik_start_run_right=loadAnimation("erik_start_run_right",config.lvlscale,BP_LD,ATYPE_ONCE);    
    anim_erik_run_left=loadAnimation("erik_run_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_erik_run_right=loadAnimation("erik_run_right",config.lvlscale,BP_LD,ATYPE_ONCE);    
    anim_erik_swim_left=loadAnimation("erik_swim_left",config.lvlscale,BP_RD,ATYPE_ONCE);         
    anim_erik_swim_right=loadAnimation("erik_swim_right",config.lvlscale,BP_LD,ATYPE_ONCE);    
    anim_erik_swim_up_left=loadAnimation("erik_swim_up_left",config.lvlscale,BP_RD,ATYPE_ONCE);
    anim_erik_swim_up_right=loadAnimation("erik_swim_up_right",config.lvlscale,BP_LD,ATYPE_ONCE);     
    anim_erik_hit_water_left=loadAnimation("erik_hit_water_left",config.lvlscale,BP_RD,ATYPE_ONCE);       
    anim_erik_hit_water_right=loadAnimation("erik_hit_water_right",config.lvlscale,BP_LD,ATYPE_ONCE);

    au_jump=scenario->sndcache->loadWAV("rboots.wav");
    au_hit=scenario->sndcache->loadWAV("erikhit.wav");
    au_run=NULL;
}

Erik::~Erik() { }

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
