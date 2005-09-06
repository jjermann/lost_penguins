#include "common.h"
#include "events.h"
#include "animation.h"
#include "weapons.h"
#include "imgcache.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "players_common.h"
#include "scenario.h"
#include "zombie.h"


Zombie::Zombie(string imagename, Sint16 xcord, Sint16 ycord, string mname):
  Monster(imagename,xcord,ycord,mname),
  au_attack(scenario->sndcache->loadWAV("clang.wav")),
  T_Attack_Bite(1500) {
    maxspeedx=80;
    anim_left=loadAnimation(scenario->imgcache->loadImage(2,"olaf1_left.bmp"),2,BP_MD,ATYPE_LOOP,2);
    anim_right=loadAnimation(scenario->imgcache->loadImage(2,"olaf1_right.bmp"),2,BP_MD,ATYPE_LOOP,2);
    weapon=Weapon(-1,W_STRIKE);
}

void Zombie::idle(Uint16 dt) {
    Character::idle(dt);
    runAI(dt);
}

void Zombie::ai_left(Uint16 dt) {
    SDL_Rect oldpos=pos;
    if ((hspeed-HSPEED_MULT*dt/100)>(-maxspeedx)) hspeed-=HSPEED_MULT*dt/100;
    else if (hspeed>(-maxspeedx)) hspeed=-maxspeedx;
    Hit hit=move(dt,true);
    if (hit.touch&enemy_types) {
        move(dt);
    } else if (hit.touch&DIR_LEFT) {
        move(dt);
        unsetState(STATE_LEFT);
    } else if (state&STATE_FALL) {
        move(dt);
    } else if (!(hit.touch&DIR_DOWN)) {
        unsetState(STATE_LEFT);
        hspeed=0;
    } else {
        move(dt);
    }
}

void Zombie::ai_right(Uint16 dt) {
    SDL_Rect oldpos=pos;
    if ((hspeed+HSPEED_MULT*dt/100)<maxspeedx) hspeed+=HSPEED_MULT*dt/100;
    else if (hspeed<maxspeedx) hspeed=maxspeedx;
    Hit hit=move(dt,true);
    if (hit.touch&enemy_types) {
        move(dt);
    } else if (hit.touch&DIR_RIGHT) {
        move(dt);
        setState(STATE_LEFT);
    } else if (state&STATE_FALL) {
        move(dt);
    } else if (!(hit.touch&DIR_DOWN)) {
        setState(STATE_LEFT);
        hspeed=0;
    } else {
        move(dt);
    }
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
                Player* target=*(targets.begin());
                Uint16 dir=scenario->getDirection(getCenter(),target->getCenter());
                dir&=~(DIR_UP|DIR_DOWN);
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

void Zombie::ai_attack(Player* plr, Uint16 dir) {
    cout << name << " attacks " << plr->getName() << " into " << dir << endl;
    sfxeng->playWAV(au_attack);
    plr->hit(dir,weapon);
    Dattack=0;
}
