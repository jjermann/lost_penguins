#include "common.h"
#include "events.h"
#include "anim.h"
#include "imgcache.h"
#include "sndcache.h"
#include "input.h"
#include "baleog.h"


Baleog::Baleog(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname) {
    im_left=new Animation(imgcache->loadImage("baleog_right.bmp"));
    im_right=im_left;
    im_fall_left=im_left;
    im_fall_right=im_left;
    im_krit_left=im_left;
    im_krit_right=im_left;
    im_land_left=new Animation(imgcache->loadImage("olaf_land.bmp"),1,T_IRR,true);
    im_land_right=im_land_left;
    weapon=Weapon(-1,W_STRIKE);
    im_sword_left=new Animation(60,imgcache->loadImage("kuru.bmp"),12,500,true);
    im_sword_right=im_sword_left;
    au_sword=sndcache->loadWAV("swrdsw2.wav");
}

Baleog::~Baleog() {
    delete im_left;
    delete im_land_left;
}

//Baleog1: Sword attack
void Baleog::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_SP1);

    if (!(state&STATE_FALL)) {
        setEvent(new EAttack(this,10,&weapon,(state&STATE_LEFT) ? DIR_LEFT : DIR_RIGHT,5,enemy_types,0,0,au_sword,(state&STATE_LEFT) ? im_sword_left : im_sword_right));
    }
}
