#include "common.h"
#include "events.h"
#include "anim.h"
#include "input.h"
#include "imgcache.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "olaf.h"


Olaf::Olaf(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname),
  fart(V_FART) {
    im_left=new Animation(imgcache->loadImage("olaf_left.bmp"));
    im_right=new Animation(imgcache->loadImage("olaf_right.bmp"));
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=new Animation(imgcache->loadImage("olaf_land.bmp"),1,T_IRR,true);
    im_land_right=im_land_left;

    im_small_left=new Animation(imgcache->loadImage("gun.bmp"),9,1000);
    im_small_right=im_small_left;
    im_shield_right=new Animation(imgcache->loadImage("olaf_fall_shield_right.bmp"));
    im_shield_left=new Animation(imgcache->loadImage("olaf_fall_shield_left.bmp"));
    im_fall_shield_left=im_shield_left;
    im_fall_shield_right=im_shield_right;
    im_die=new Animation(60,imgcache->loadImage("kuru.bmp"),12,2000,true);
    au_small=sndcache->loadWAV("blob.wav");
    au_big=sndcache->loadWAV("unblob.wav");
    au_fart=sndcache->loadWAV("fart1.wav");
    au_hit=sndcache->loadWAV("fathit.wav");
}
Olaf::~Olaf() {
    delete im_left;
    delete im_right;
    delete im_land_left;
    delete im_small_left;
    delete im_shield_right;
    delete im_shield_left;
}

void Olaf::updateAnimState(bool change) {
    if (!change) {
    } else if (state&STATE_SMALL) {
        if (state&STATE_LEFT) animation=im_small_left;
        else animation=im_small_right;
    } else if (state&STATE_SHIELD) {
        otype|=OTYPE_DENSE_D;
        if (state&STATE_LEFT) {
            if (state&STATE_FALL) {
                animation=im_fall_shield_left;
            } else {
                animation=im_shield_left;
            }
        } else {
            if (state&STATE_FALL) {
                animation=im_fall_shield_right;
            } else {
                animation=im_shield_right;
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

void Olaf::in_left(Sint16 dt) {
    //No navigation while farting from ground (replace this by a busy event)
    if ((dt < 0) || (!(state&STATE_ACT_2))) {
        Player::in_left(dt);
    }
}
void Olaf::in_right(Sint16 dt) {
    //No navigation while farting from ground (replace this by a busy event)
    if ((dt < 0) || (!(state&STATE_ACT_2))) {
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
        } else {
            sfxeng->playWAV(au_big);
            unsetState(STATE_SMALL);
        }
        return true;
    } else return false;
}

void Olaf::in_down(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_DOWN);
    //TODO: ladder
    if (!(state&STATE_FALL)) trySmall(true);
}

void Olaf::in_up(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_UP);
    //TODO: ladder
    if (!(state&STATE_FALL)) trySmall(false);
}

void Olaf::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_SP1);
    //no specials while small
    if (!(state&STATE_SMALL)) {
        switchState(STATE_SHIELD);
    }
}

void Olaf::in_sp2(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_SP2);
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
            setEvent(new ESpeed(this,DE_JUMP,fart,0,0,0,au_fart));
        }
    }
}
    
void Olaf::fall(Uint16 dt) {
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
