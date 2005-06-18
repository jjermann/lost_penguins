#include "common.h"
#include "events.h"
#include "animation.h"
#include "input.h"
#include "imgcache.h"
#include "sndcache.h"
#include "scenario.h"
#include "sfxeng.h"
#include "olaf.h"


Olaf::Olaf(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname),
  fart(V_FART) {
    im_left=new Animation(scenario->imgcache->loadImage("olaf1_left.bmp"));
    im_right=new Animation(scenario->imgcache->loadImage("olaf1_right.bmp"));
    im_run_left=new Animation(scenario->imgcache->loadImage("olaf1-run_left.png"),8,1000);
    im_run_right=new Animation(scenario->imgcache->loadImage("olaf1-run_right.png"),8,1000);
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=new Animation(scenario->imgcache->loadImage("olaf1_land_left.bmp"),1,T_IRR,true);
    im_land_right=new Animation(scenario->imgcache->loadImage("olaf1_land_right.bmp"),1,T_IRR,true);

    im_small_left=new Animation(scenario->imgcache->loadImage("Olaf_Small_Walk_left.png"),7,0);
    im_small_right=new Animation(scenario->imgcache->loadImage("Olaf_Small_Walk_right.png"),7,0);
    im_run_small_left=new Animation(scenario->imgcache->loadImage("Olaf_Small_Walk_left.png"),7,500);
    im_run_small_right=new Animation(scenario->imgcache->loadImage("Olaf_Small_Walk_right.png"),7,500);
    im_shield_right=new Animation(scenario->imgcache->loadImage("olaf1_fall_shield_right.bmp"));
    im_shield_left=new Animation(scenario->imgcache->loadImage("olaf1_fall_shield_left.bmp"));
    im_run_shield_right=im_shield_right;
    im_run_shield_left=im_shield_left;
    im_fall_shield_left=im_shield_left;
    im_fall_shield_right=im_shield_right;
    im_die=new Animation(60,scenario->imgcache->loadImage("kuru.bmp"),12,2000,true);
    au_small=scenario->sndcache->loadWAV("blob.wav");
    au_big=scenario->sndcache->loadWAV("unblob.wav");
    au_fart=scenario->sndcache->loadWAV("fart1.wav");
    au_hit=scenario->sndcache->loadWAV("fathit.wav");
}
Olaf::~Olaf() {
    delete im_left;
    delete im_right;
    delete im_run_left;
    delete im_run_right;
    delete im_land_left;
    delete im_land_right;
    delete im_small_left;
    delete im_small_right;
    delete im_run_small_left;
    delete im_run_small_right;
    delete im_shield_right;
    delete im_shield_left;
}

void Olaf::updateAnimState(bool change) {
    if (!change) {
    } else if (state&STATE_SMALL) {
        if (state&STATE_LEFT) {
            if (state&STATE_MLEFT) animation=im_run_small_left;
            else animation=im_small_left;
        } else {
            if (state&STATE_MRIGHT) animation=im_run_small_right;
            else animation=im_small_right;
        }
    } else if (state&STATE_SHIELD) {
        otype|=OTYPE_DENSE_D;
        if (state&STATE_LEFT) {
            if (state&STATE_FALL) {
                animation=im_fall_shield_left;
            } else {
                if (state&STATE_MLEFT) animation=im_run_shield_left;
                else animation=im_shield_left;
            }
        } else {
            if (state&STATE_FALL) {
                animation=im_fall_shield_right;
            } else {
                if (state&STATE_MRIGHT) animation=im_run_shield_right;
                else animation=im_shield_right;
            }
        }
    } else {
        otype&=~OTYPE_DENSE_D;
        Player::updateAnimState();
    }
    curpos.w=animation->getWidth();
    curpos.h=animation->getHeight();
    curpos.x=(Uint16)((pos.w-curpos.w)/2);
    curpos.y=(pos.h-curpos.h);
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

    Animation* tmpanim;
    SDL_Rect tmppos=pos;
    if (small) tmpanim=im_small_left;
    //Assume both images have the same dimension
    else tmpanim=im_orig;
    //IDEA: left/right edge instead of bottom?
    tmppos.x+=(Sint16)((tmppos.w-tmpanim->getWidth())/2);
    tmppos.y+=tmppos.h-tmpanim->getHeight();
    tmppos.w=tmpanim->getWidth();
    tmppos.h=tmpanim->getHeight();
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
        if (!getState(STATE_FALL)) hspeed=boost(hspeed,-dt*HSPEED_MULT/100);
        else hspeed=boost(hspeed,-dt*HSPEED_MULT/200);
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
