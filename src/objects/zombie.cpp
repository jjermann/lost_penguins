#include "common.h"
#include "events.h"
#include "anim.h"
#include "weapons.h"
#include "imgcache.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "vikings_common.h"
#include "map.h"
#include "zombie.h"


Zombie::Zombie(string imagename, Uint16 xcord, Uint16 ycord, string mname):
  Monster(imagename,xcord,ycord,mname) {
    maxspeedx=100;
    T_Attack_Bite=1500;
    im_left=new Animation(imgcache->loadImage("olaf_left.bmp"),2,1000);
    im_right=new Animation(imgcache->loadImage("olaf_right.bmp"),2,1000);
    weapon=Weapon(-1,W_STRIKE);
    au_attack=sndcache->loadWAV("clang.wav");
}

Zombie::~Zombie() {
    delete im_left;
    delete im_right;
}

void Zombie::idle(Uint16 dt) {
    hspeed=0;
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
                Uint16 dir=curmap->getDirection(getCenter(),target->getCenter());
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

void Zombie::ai_attack(Viking* vik, Uint16 dir) {
    cout << name << " attacks " << vik->getName() << " into " << dir << endl;
    sfxeng->playWAV(au_attack);
    vik->hit(dir,weapon);
    Dattack=0;
}
