#include "common.h"
#include "events.h"
#include "animation.h"
#include "input.h"
#include "imgcache.h"
#include "sndcache.h"
#include "scenario.h"
#include "sfxeng.h"
#include "olaf.h"


Olaf::Olaf(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Player(xcord,ycord,param),
  fart(V_FART) {
    /* Parameters */
    if (hasParam(parameters,"anim_olaf_small_left"))          anim_olaf_small_left=loadAnimation(getParameters(parameters["anim_olaf_small_left"],':'));
      else anim_olaf_small_left=loadAnimation(scenario->imgcache->loadImage(7,"Olaf_Small_Walk_left.png"),1);
    if (hasParam(parameters,"anim_olaf_small_right"))         anim_olaf_small_right=loadAnimation(getParameters(parameters["anim_olaf_small_right"],':'));
      else anim_olaf_small_right=loadAnimation(scenario->imgcache->loadImage(7,"Olaf_Small_Walk_right.png"),1);
    if (hasParam(parameters,"anim_olaf_walk_small_left"))     anim_olaf_walk_small_left=loadAnimation(getParameters(parameters["anim_olaf_walk_small_left"],':'));
      else anim_olaf_walk_small_left=loadAnimation(scenario->imgcache->loadImage(7,"Olaf_Small_Walk_left.png"),7,BP_MD,ATYPE_LOOP,3.5);
    if (hasParam(parameters,"anim_olaf_walk_small_right"))    anim_olaf_walk_small_right=loadAnimation(getParameters(parameters["anim_olaf_walk_small_right"],':'));
      else anim_olaf_walk_small_right=loadAnimation(scenario->imgcache->loadImage(7,"Olaf_Small_Walk_right.png"),7,BP_MD,ATYPE_LOOP,3.5);
    if (hasParam(parameters,"anim_olaf_shield_left"))         anim_olaf_shield_left=loadAnimation(getParameters(parameters["anim_olaf_shield_left"],':'));
      else anim_olaf_shield_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_fall_shield_left.bmp"));
    if (hasParam(parameters,"anim_olaf_shield_right"))        anim_olaf_shield_right=loadAnimation(getParameters(parameters["anim_olaf_shield_right"],':'));
      else anim_olaf_shield_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_fall_shield_right.bmp"));
    if (hasParam(parameters,"anim_olaf_walk_shield_left"))    anim_olaf_walk_shield_left=loadAnimation(getParameters(parameters["anim_olaf_walk_shield_left"],':'));
      else anim_olaf_walk_shield_left.reset(new EmptyAnimation(&anim_olaf_shield_left));
    if (hasParam(parameters,"anim_olaf_walk_shield_right"))   anim_olaf_walk_shield_right=loadAnimation(getParameters(parameters["anim_olaf_walk_shield_right"],':'));
      else anim_olaf_walk_shield_right.reset(new EmptyAnimation(&anim_olaf_shield_right));
    if (hasParam(parameters,"anim_olaf_fall_shield_left"))    anim_olaf_fall_shield_left=loadAnimation(getParameters(parameters["anim_olaf_fall_shield_left"],':'));
      else anim_olaf_fall_shield_left.reset(new EmptyAnimation(&anim_olaf_shield_left));
    if (hasParam(parameters,"anim_olaf_fall_shield_right"))   anim_olaf_fall_shield_right=loadAnimation(getParameters(parameters["anim_olaf_fall_shield_right"],':'));
      else anim_olaf_fall_shield_right.reset(new EmptyAnimation(&anim_olaf_shield_right));

    if (!hasParam(parameters,"audio_hit")) au_hit=scenario->sndcache->loadWAV("fathit.wav");

    if (hasParam(parameters,"audio_small")) au_small=scenario->sndcache->loadWAV(parameters["audio_small"]);
      else au_small=scenario->sndcache->loadWAV("blob.wav");
    if (hasParam(parameters,"audio_big")) au_big=scenario->sndcache->loadWAV(parameters["audio_big"]);
      else au_big=scenario->sndcache->loadWAV("unblob.wav");
    if (hasParam(parameters,"audio_fart")) au_fart=scenario->sndcache->loadWAV(parameters["audio_fart"]);
      else au_fart=scenario->sndcache->loadWAV("fart1.wav");

    normal_size=pos;
    small_size=anim_olaf_small_right->getFrameDim();
}

Olaf::~Olaf() { }

void Olaf::updateAnimState() {
    if (state&STATE_SMALL) {
        if (state&STATE_LEFT) {
            anim_olaf_small_left->setFallBack(&anim_walk_left);
            setAnim(anim_olaf_walk_small_left);
            anim_olaf_small_left->setFallBack(&anim_left);
        } else {
            anim_olaf_small_right->setFallBack(&anim_walk_right);
            setAnim(anim_olaf_walk_small_right);
            anim_olaf_small_right->setFallBack(&anim_right);
        }
    } else if (state&STATE_SHIELD) {
        otype|=OTYPE_DENSE_D;
        if (state&STATE_LEFT) {
            if (state&STATE_FALL) {
                anim_olaf_shield_left->setFallBack(&anim_fall_left);
                setAnim(anim_olaf_fall_shield_left);
                anim_olaf_shield_left->setFallBack(&anim_left);
            } else {
                if (state&STATE_MLEFT) {
                    anim_olaf_shield_left->setFallBack(&anim_walk_left);
                    setAnim(anim_olaf_walk_shield_left);
                    anim_olaf_shield_left->setFallBack(&anim_left);
                } else {
                    setAnim(anim_olaf_shield_left);
                }
            }
        } else {
            if (state&STATE_FALL) {
                anim_olaf_shield_right->setFallBack(&anim_fall_right);
                setAnim(anim_olaf_fall_shield_right);
                anim_olaf_shield_right->setFallBack(&anim_right);
            } else {
                if (state&STATE_MRIGHT) {
                    anim_olaf_shield_right->setFallBack(&anim_walk_right);
                    setAnim(anim_olaf_walk_shield_right);
                    anim_olaf_shield_right->setFallBack(&anim_right);
                } else {
                    setAnim(anim_olaf_shield_right);
                }
            }
        }
    } else {
        otype&=~OTYPE_DENSE_D;
        Player::updateAnimState();
    }
}

void Olaf::in_left(Uint16 dt) {
    //No navigation while farting from ground (replace this by a busy event)
    if (!(state&STATE_ACT_2)) {
        Player::in_left(dt);
    }
}
void Olaf::in_right(Uint16 dt) {
    //No navigation while farting from ground (replace this by a busy event)
    if (!(state&STATE_ACT_2)) {
        Player::in_right(dt);
    }
}

inline bool Olaf::trySmall(bool small) {
    //Are we already small?
    if ((small && (state&STATE_SMALL)) || ((!small) && (!(state&STATE_SMALL)))) return true;

    SDL_Rect tmppos=pos;
    //Assume both images have the same dimension
    //IDEA: left/right edge instead of bottom?
    tmppos.x+=(Sint16)((tmppos.w-(small ? small_size.w : normal_size.w))/2);
    tmppos.y+=tmppos.h-(small ? small_size.h : normal_size.h);
    tmppos.w=(small ? small_size.w : normal_size.w);
    tmppos.h=(small ? small_size.h : normal_size.h);
    if (!(checkMove(tmppos,true).enter&DIR_ALL)) {
        pos=tmppos;
        if (small) {
            sfxeng->playWAV(au_small);
            //as we don't fall most states are off anyway
            unsetState(STATE_SHIELD);
            setState(STATE_SMALL);
            addMaxSpeed(-200);
        } else {
            sfxeng->playWAV(au_big);
            unsetState(STATE_SMALL);
            addMaxSpeed(200);
        }
        return true;
    } else return false;
}

void Olaf::in_down() {
    //TODO: ladder
    if (!(state&STATE_FALL)) trySmall(true);
}

void Olaf::in_up() {
    //TODO: ladder
    if (!(state&STATE_FALL)) trySmall(false);
}

void Olaf::in_sp1() {
    //no specials while small
    if (!(state&STATE_SMALL)) {
        switchState(STATE_SHIELD);
    }
}

void Olaf::in_sp2() {
    //no specials while small
    if (!(state&STATE_SMALL)) {
        //Don't fart while falling without shield
        if ((state&STATE_FALL) && (!(state&STATE_SHIELD)) ) return;
        //farting from ground
        else if (!(state&STATE_FALL)) {
            setState(STATE_ACT_2);
        }
        //Can't fart anymore
        if (state&STATE_ACT_1) {
        //Fart
        } else {
            setState(STATE_ACT_1);
            addSpeed(fart);
            setEvent(new CAnimEvent(this,DE_JUMP,0,0,au_fart));
        }
    }
}
    
void Olaf::fall(Uint16 dt) {
    if (!getState(STATE_MRIGHT|STATE_MLEFT)) {
        if (!getState(STATE_FALL)) hspeed=addAbsolute(hspeed,-dt*HSPEED_MULT/100);
        else hspeed=addAbsolute(hspeed,-dt*HSPEED_MULT/200);
    }
    Dgrav+=dt;
    if (Dgrav>T_GRAV_EFFECT) {
        if (state&STATE_FALL) {
            addSpeed(Sint16(gravity*Dgrav/1000));
            if ((state&STATE_SMALL) && (speed > V_SMALLCHANGE)) trySmall(false);
            if ((state&STATE_SHIELD) && (speed > V_SHIELD)) setSpeed(V_SHIELD);
        }
        Hit hit=move(Dgrav);
        if ((hit.enter&DIR_DOWN) && (state&STATE_FALL)) crash(DIR_DOWN);
        Dgrav=0;
    }
}

Uint16 Olaf::hit(Uint16 dir, Weapon& weap) {
    //TODO fix the small->big thing...
    trySmall(false);
    if ((!(state&STATE_SMALL)) && (weap.getType()&(W_STRIKE|W_EXPLOSION))) {
        if (state&STATE_SHIELD) {
            if (dir==DIR_DOWN) return health;
            else return Player::hit(dir,weap);
        } else if (state&STATE_LEFT) {
            if (dir==DIR_RIGHT) return health;
            else return Player::hit(dir,weap);
        } else {
            if (dir==DIR_LEFT) return health;
            else return Player::hit(dir,weap);
        }
    } else return Player::hit(dir,weap);
}
