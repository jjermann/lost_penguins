#include "common.h"
#include "events.h"
#include "animation.h"
#include "input.h"
#include "imgcache.h"
#include "sndcache.h"
#include "scenario.h"
#include "fang.h"


Fang::Fang(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Player(xcord,ycord,param),
  jump(V_JUMP) {
    weapon=Weapon(-1,W_STRIKE);

    /* Parameters */
    if (!hasParam(parameters,"anim_left"))          anim_left=loadAnimation(scenario->imgcache->loadImage(4,"Fang_Breath_left.png"),4);
    if (!hasParam(parameters,"anim_right"))         anim_right=loadAnimation(scenario->imgcache->loadImage(4,"Fang_Breath_right.png"),4);
    if (!hasParam(parameters,"anim_walk_left"))     anim_walk_left=loadAnimation(scenario->imgcache->loadImage(8,"Fang_walk_left.png"),8);
    if (!hasParam(parameters,"anim_walk_right"))    anim_walk_right=loadAnimation(scenario->imgcache->loadImage(8,"Fang_walk_right.png"),8);
    if (!hasParam(parameters,"anim_crash_left"))    anim_crash_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),1,BP_MD,ATYPE_ONCE,calcFPS(1,T_IRR));
    if (!hasParam(parameters,"anim_crash_right"))   anim_crash_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),1,BP_MD,ATYPE_ONCE,calcFPS(1,T_IRR));

    if (hasParam(parameters,"anim_fang_claw_left")) anim_fang_claw_left=loadAnimation(getParameters(parameters["anim_fang_claw_left"],':'));
      else anim_fang_claw_left=loadAnimation(scenario->imgcache->loadImage(8,"Fang_Clawslash_left.png"),8,BP_MD,ATYPE_ONCE);
    if (hasParam(parameters,"anim_fang_claw_right")) anim_fang_claw_right=loadAnimation(getParameters(parameters["anim_fang_claw_right"],':'));
      else anim_fang_claw_right=loadAnimation(scenario->imgcache->loadImage(8,"Fang_Clawslash_right.png"),8,BP_MD,ATYPE_ONCE);

    if (!hasParam(parameters,"audio_hit")) au_hit=scenario->sndcache->loadWAV("wolfhit.wav");

    if (hasParam(parameters,"audio_claw")) au_claw=scenario->sndcache->loadWAV(parameters["audio_claw"]);
      else au_claw=scenario->sndcache->loadWAV("wolfjmp1.wav");
    if (hasParam(parameters,"audio_jump")) au_jump=scenario->sndcache->loadWAV(parameters["audio_jump"]);
      else au_jump=scenario->sndcache->loadWAV("fangjump.wav");
}

Fang::~Fang() { }

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
        if (!getState(STATE_FALL)) hspeed=addAbsolute(hspeed,-dt*HSPEED_MULT/100);
        else hspeed=addAbsolute(hspeed,-dt*HSPEED_MULT/200);
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
    setEvent(new EAttack(this,10,&weapon,(state&STATE_LEFT) ? DIR_LEFT : DIR_RIGHT,10,enemy_types,0,0,au_claw,(state&STATE_LEFT) ? anim_fang_claw_left : anim_fang_claw_right));
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
