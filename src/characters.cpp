#include "lost_vikings.h"
#include "events.h"
#include "objects.h"
#include "weapons.h"

using namespace std;

/*==================
 Common Characters
==================*/

TriggeredBomb::TriggeredBomb(string imagename, Uint16 xcord, Uint16 ycord, Uint16 tleft, string cname):
  Character(imagename,xcord,ycord,cname), countdown(tleft) {
    weapon=Weapon(-1,W_EXPLOSION);
    au_bomb=loadWAV("explsn.wav");
}
TriggeredBomb::~TriggeredBomb() {
    if (au_bomb) Mix_FreeChunk(au_bomb);
}

void TriggeredBomb::idle(Uint16 dt) {
    countdown-=dt;
    if (countdown<=0) {
        cout << "Booom!\n";
        sfxeng->playWAV(au_bomb);
        std::set<Character *> cset=map->getCharactersIn(pos,true,20);
        character_iterator cit=cset.begin();
        while (cit!=cset.end()) {
            (*cit)->hit(map->getDirection(pos,*(*cit)->getPos()),weapon);
            ++cit;
        }
        die();
    }
}


/*==================
      Vikings
==================*/

//ERIC (Viking)
Eric::Eric(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Viking(imagename,xcord,ycord,vname) {
    im_left=new Animation(loadImage("eric_left.bmp"));
    im_right=new Animation(loadImage("kuru.bmp"),60,12,1000);
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=im_left;
    im_land_right=im_right;
    au_jump=loadWAV("rboots.wav");
    au_run=NULL;
    t_water=60000;
    jump=V_JUMP;
    jump2=V_JUMP2;
}
Eric::~Eric() {
    delete im_left;
    delete im_right;
    if (au_jump) Mix_FreeChunk(au_jump);
    if (au_run) Mix_FreeChunk(au_run);
}

//Eric1: Jump
//Act1: first jump
//Act2: 2nd jump
void Eric::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_SP1);
    if (state&STATE_FALL) setState(STATE_ACT_1);

    if (state&STATE_ACT_2) {
    } else if (state&STATE_ACT_1) {
        setState(STATE_ACT_2);
        setEvent(new ESpeed(this,DE_JUMP,jump2,0,0,0,au_jump));
    } else {
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,DE_JUMP,jump));
    }
}

//TODO: change this to a horizontal gravitiy modifier
void Eric::in_sp2(Sint16 dt) {
    if (state&STATE_RUN) {
        if ((dt < 0) || (state&STATE_FALL)) cancelEvent();
    } else {
        if ((dt < 0) || (state&STATE_FALL)) {
        } else if (state&STATE_MLEFT) {
            setEvent(new ERun(this,10000,Sint16(-1/2*maxspeedx),-maxspeedx,500,ESTATE_ABORT,au_run));
        } else if (state&STATE_MRIGHT) {
            setEvent(new ERun(this,10000,Sint16(1/2*maxspeedx),maxspeedx,500,ESTATE_ABORT,au_run));
        }
    }
}

void Eric::in_left(Sint16 dt) {
    if (state&STATE_RUN) {
        if (dt < 0) {
            cancelEvent();
        } else {
            if (state&STATE_LEFT) event->reset();
            else cancelEvent();
        }
    }
    Viking::in_left(dt);
}

void Eric::in_right(Sint16 dt) {
    if (state&STATE_RUN) {
        if (dt < 0) {
            cancelEvent();
        } else {
            if (state&STATE_LEFT) cancelEvent();
            else event->reset();
        }
    }
    Viking::in_right(dt);
}


//OLAF (Viking)
Olaf::Olaf(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Viking(imagename,xcord,ycord,vname) {
    im_left=new Animation(loadImage("olaf_left.bmp"));
    im_right=new Animation(loadImage("olaf_right.bmp"));
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=new Animation(loadImage("olaf_land.bmp"));
    im_land_right=im_land_left;

    im_small_left=new Animation(loadImage("gun.bmp"),9,1000);
    im_small_right=im_small_left;
    im_shield_right=new Animation(loadImage("olaf_fall_shield_right.bmp"));
    im_shield_left=new Animation(loadImage("olaf_fall_shield_left.bmp"));
    im_fall_shield_left=im_shield_left;
    im_fall_shield_right=im_shield_right;
    au_small=loadWAV("blob.wav");
    au_big=loadWAV("unblob.wav");
    au_fart=loadWAV("fart1.wav");
    au_hit=loadWAV("fathit.wav");
    fart=V_FART;
}
Olaf::~Olaf() {
    delete im_left;
    delete im_right;
    delete im_land_left;
    delete im_small_left;
    delete im_shield_right;
    delete im_shield_left;
    if (au_small) Mix_FreeChunk(au_small);
    if (au_big) Mix_FreeChunk(au_big);
    if (au_fart) Mix_FreeChunk(au_fart);
}

void Olaf::updateAnimState() {
    if (state&STATE_SMALL) {
        if (state&STATE_LEFT) animation=im_small_left;
        else animation=im_small_right;
        curpos.w=animation->getWidth();
        curpos.h=animation->getHeight();
        curpos.x=(Uint16)((pos.w-curpos.w)/2);
        curpos.y=(pos.h-curpos.h);
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
        curpos.w=animation->getWidth();
        curpos.h=animation->getHeight();
        curpos.x=(Uint16)((pos.w-curpos.w)/2);
        curpos.y=(pos.h-curpos.h);
    } else {
        otype&=~OTYPE_DENSE_D;
        Viking::updateAnimState();
    }
}

void Olaf::in_left(Sint16 dt) {
    if (dt < 0) {
        Viking::in_left(dt);
        return;
    }
    //No navigation while farting from ground
    if (!(state&STATE_ACT_2)) {
        Viking::in_left(dt);
    }
}
void Olaf::in_right(Sint16 dt) {
    if (dt < 0) {
        Viking::in_right(dt);
        return;
    }
    //No navigation while farting from ground
    if (!(state&STATE_ACT_2)) {
        Viking::in_right(dt);
    }
}

//try to change the small state to small, returns true if successfull
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
    //no ladder (TODO: ladder)
    input->unsetState(INPUT_DOWN);
    if (!(state&STATE_FALL)) trySmall(true);
}

void Olaf::in_up(Sint16 dt) {
    if (dt < 0) return;
    //no ladder (TODO: ladder)
    input->unsetState(INPUT_UP);
    if (!(state&STATE_FALL)) trySmall(false);
}

//Olaf1: Shield
void Olaf::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    //no specials while small
    if (!(state&STATE_SMALL)) {
        input->unsetState(INPUT_SP1);
        switchState(STATE_SHIELD);
    }
}

//Olaf2: Fart
//Act1: farted
//Act2: no left, right movement (farting from ground)
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
        if ((hit.enter&DIR_DOWN || hit.touch&DIR_DOWN) && (state&STATE_FALL)) land();
        Dgrav=0;
    }
}

Sint16 Olaf::hit(Uint16 dir, Weapon& weap) {
    //check if olaf is protected by a shield or falling
    Sint16 dhealth=0;
    if (!((state&STATE_SHIELD) || (state&STATE_FALL))) {
        if ((!(state&STATE_LEFT)) && (dir&DIR_LEFT)) dhealth=1;
        else if ((state&STATE_LEFT) && (dir&DIR_RIGHT)) dhealth=1;
    }
    if (dhealth) return 0;
    else dhealth=addHealth(weap.getDamage());
    
    switch (weap.getType()) {   
        case W_STRIKE: {
            if (dir&DIR_RIGHT) setEvent(new ESpeed(this,TSTRIKE,-60,DSTRIKE,0,0,au_hit));
            if (dir&DIR_LEFT)  setEvent(new ESpeed(this,TSTRIKE,-60,-DSTRIKE,0,0,au_hit));
            break;
        }
        default: {
            break;
        }
    }
    return dhealth;
}


//SCORCH (Viking)
Scorch::Scorch(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Viking(imagename,xcord,ycord,vname) {
    im_left=new Animation(loadImage("baleog_right.bmp"));
    im_right=im_left;
    im_fall_left=im_left;
    im_fall_right=im_left;
    im_krit_left=im_left;
    im_krit_right=im_left;
    im_land_left=im_left;
    im_land_right=im_right;
    au_swing=NULL;
    au_tired=NULL;
    left_wings=SCORCH_MAX_WINGS;
    wing=V_FLY;
}
Scorch::~Scorch() {
    delete im_left;
    if (au_swing) Mix_FreeChunk(au_swing);
    if (au_tired) Mix_FreeChunk(au_tired);
}

void Scorch::idle(Uint16 dt) {
    Viking::idle(dt);
    if (!(input->getState(INPUT_SP1))) {
        unsetState(STATE_ACT_1);
        unsetState(STATE_GLIDE);
    }
}

void Scorch::fall(Uint16 dt) {
    Dgrav+=dt;
    if (Dgrav>T_GRAV_EFFECT) {
        if (state&STATE_FALL) {
            addSpeed(Sint16(gravity*Dgrav/1000));
            if ((state&STATE_GLIDE) && (speed > V_GLIDE)) speed=V_GLIDE;;
        }
        Hit hit=move(Dgrav);
        if ((hit.enter&DIR_DOWN || hit.touch&DIR_DOWN) && (state&STATE_FALL)) land();
        Dgrav=0;
    }
}

//Scorch1: Fly
//Act1: no wings, but key down
//Act2: exhausted
void Scorch::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    setState(STATE_FALL);
    //if not exhausted -> glide
    if (!(state&STATE_ACT_2)) {
        setState(STATE_GLIDE);
    } else {
    //TODO: add glide2?
    }
    //Can't fly anymore
    if (state&STATE_ACT_2) {
    } else if (left_wings<=0) {
        left_wings--;
        setState(STATE_ACT_2);
        unsetState(STATE_GLIDE);
        setState(STATE_ACT_1);
    //Use Wings
    } else if (!(state&STATE_ACT_1)){
        left_wings--;
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,DE_WING,V_FLY,0,0,0,au_swing));
    }
}

void Scorch::land() {
    unsetState(STATE_GLIDE);
    input->unsetState(INPUT_SP1);
    left_wings=SCORCH_MAX_WINGS;
    Viking::land();
}


//FANG (Viking)
Fang::Fang(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Viking(imagename,xcord,ycord,vname) {
    im_left=new Animation(loadImage("olaf_left.bmp"));
    im_right=new Animation(loadImage("olaf_right.bmp"));
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=new Animation(loadImage("olaf_land.bmp"));
    im_land_right=im_land_left;
    au_jump=NULL;
    jump=V_JUMP;
}
Fang::~Fang() {
    delete im_left;
    delete im_right;
    delete im_land_left;
    if (au_jump) Mix_FreeChunk(au_jump);
}

void Fang::in_left(Sint16 dt) {
    Viking::in_left(dt);
    if (dt >= 0) unsetState(STATE_CLIMB_R);
}
void Fang::in_right(Sint16 dt) {
    Viking::in_right(dt);
    if (dt >= 0) unsetState(STATE_CLIMB_L);
}

void Fang::fall(Uint16 dt) {
    Dgrav+=dt;
    if (Dgrav>T_GRAV_EFFECT) {
        if (state&STATE_FALL) {
            addSpeed(Sint16(gravity*Dgrav/1000));
            if ((state&STATE_CLIMB) && (speed>V_CLIMB)) speed=V_CLIMB;
        }
        Hit hit=move(Dgrav);
        if (state&STATE_FALL) {
            //Did we hit the wall?
            if (hit.enter&DIR_LEFT) {
                setState(STATE_CLIMB_L);
            } else if (!(hit.touch&DIR_LEFT)) unsetState(STATE_CLIMB_L);
            if (hit.enter&DIR_RIGHT) {
                setState(STATE_CLIMB_R);
            } else if (!(hit.touch&DIR_RIGHT)) unsetState(STATE_CLIMB_R);
        }
        if ((hit.enter&DIR_DOWN || hit.touch&DIR_DOWN) && (state&STATE_FALL)) land();
        Dgrav=0;
    }
}

//Fang1: Jump
//Act1: jumped
void Fang::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_SP1);

    if (state&STATE_CLIMB_L) {
        unsetState(STATE_LEFT);
        unsetState(STATE_CLIMB_L);
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,T_JUMPOFF,V_JUMPOFF,Sint16(maxspeedx*1.5),0,0,au_jump));
    } else if (state&STATE_CLIMB_R) {
        setState(STATE_LEFT);
        unsetState(STATE_CLIMB_R);
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,T_JUMPOFF,V_JUMPOFF,Sint16(-maxspeedx*1.5),0,0,au_jump));
    } else if ((!(state&STATE_FALL)) && (!(state&STATE_ACT_1))){
        setState(STATE_ACT_1);
        setEvent(new ESpeed(this,DE_JUMP,V_JUMP,0,0,0,au_jump));
    }
}

void Fang::land() {
    unsetState(STATE_CLIMB_R);
    unsetState(STATE_CLIMB_L);
    Viking::land();
}


//BALEOG (Viking)
Baleog::Baleog(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Viking(imagename,xcord,ycord,vname) {
    im_left=new Animation(loadImage("baleog_right.bmp"));
    im_right=im_left;
    im_fall_left=im_left;
    im_fall_right=im_left;
    im_krit_left=im_left;
    im_krit_right=im_left;
    im_land_left=im_left;
    im_land_right=im_right;
    au_hit=NULL;
}

Baleog::~Baleog() {
    delete im_left;
    if (au_hit) Mix_FreeChunk(au_hit);
}


/*==================
      Monsters
==================*/

//ZOMBIE (Monster)
Zombie::Zombie(string imagename, Uint16 xcord, Uint16 ycord, string mname):
  Monster(imagename,xcord,ycord,mname) {
    maxspeedx=100;
    T_Attack_Bite=1500;
    im_left=new Animation(loadImage("olaf_left.bmp"),2,1000);
    im_right=new Animation(loadImage("olaf_right.bmp"),2,1000);
    weapon=Weapon(-1,W_STRIKE);
    au_attack=loadWAV("clang.wav");
}

Zombie::~Zombie() {
    delete im_left;
    delete im_right;
}

void Zombie::idle(Uint16 dt) {
    Character::idle(dt);
    if (!(state&STATE_FALL)) {
        runAI(dt);
    }
}

void Zombie::ai_left(Uint16 dt) {
    SDL_Rect oldpos=pos;
    hspeed-=maxspeedx;
    Hit hit=move(dt,true);
    if (hit.touch&enemy_types) {
        move(dt);
    } else if (hit.touch&DIR_LEFT) {
        move(dt);
        unsetState(STATE_LEFT);
    } else if (!(hit.touch&DIR_DOWN)) {
        unsetState(STATE_LEFT);
        hspeed=0;
    } else move(dt);
}

void Zombie::ai_right(Uint16 dt) {
    SDL_Rect oldpos=pos;
    hspeed+=maxspeedx;
    Hit hit=move(dt,true);
    if (hit.touch&enemy_types) {
        move(dt);
    } else if (hit.touch&DIR_RIGHT) {
        move(dt);
        setState(STATE_LEFT);
    } else if (!(hit.touch&DIR_DOWN)) {
        setState(STATE_LEFT);
        hspeed=0;
    } else move(dt);
}

void Zombie::runAI(Uint16 dt) {
    Dai+=dt;
    if (state&STATE_ATTACK) Dattack+=dt;
    else Dattack=T_Attack_Bite-300;
        
    if (Dai>T_AI_EFFECT) {
        if (state&STATE_ATTACK) {
            if (targets.empty()) {
                //shouldn't happen
                unsetState(STATE_ATTACK);
            } else {
                Viking* target=*(targets.begin());
                Uint16 dir=map->getDirection(getCenter(),target->getCenter());
                if (dir&DIR_LEFT) setState(STATE_LEFT);
                else unsetState(STATE_LEFT);
                
                if (Dattack>T_Attack_Bite) {
                    ai_attack(target,dir);
                }
            }
        } else if (state&STATE_LEFT) ai_left(Dai);
        else ai_right(Dai);
        Dai=0;
    }
}

void Zombie::ai_attack(Viking* vik, Uint16 dir) {
    cout << name << " attacks " << vik->getName() << " into " << dir << endl;
    sfxeng->playWAV(au_attack);
    vik->hit(dir,weapon);
    Dattack=0;
}
