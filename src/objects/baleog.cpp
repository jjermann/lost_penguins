#include "common.h"
#include "events.h"
#include "anim.h"
#include "imgcache.h"
#include "sndcache.h"
#include "input.h"
#include "baleog.h"


Baleog::Baleog(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname) {
    im_left=new Animation(imgcache->loadImage("baleog_left.bmp"));
    im_right=new Animation(imgcache->loadImage("baleog_right.bmp"));
    im_run_left=new Animation(imgcache->loadImage("baleog-run_left.png"),8,1000);
    im_run_right=new Animation(imgcache->loadImage("baleog-run_right.png"),8,1000);
    im_fall_left=im_left;
    im_fall_right=im_left;
    im_krit_left=im_left;
    im_krit_right=im_left;
    im_land_left=new Animation(imgcache->loadImage("olaf_land_left.bmp"),1,T_IRR,true);
    im_land_right=new Animation(imgcache->loadImage("olaf_land_right.bmp"),1,T_IRR,true);
    weapon=Weapon(-1,W_STRIKE);
    im_sword_left=new Animation(60,imgcache->loadImage("kuru.bmp"),12,500,true);
    im_sword_right=im_sword_left;
    au_sword=sndcache->loadWAV("swrdsw2.wav");
}

Baleog::~Baleog() {
    delete im_left;
    delete im_right;
    delete im_run_left;
    delete im_run_right;
    delete im_land_left;
    delete im_land_right;
    delete im_sword_left;
}

//Baleog1: Sword attack
void Baleog::in_sp1(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_SP1);

    setEvent(new EAttack(this,10,&weapon,(state&STATE_LEFT) ? DIR_LEFT : DIR_RIGHT,10,enemy_types,0,0,au_sword,(state&STATE_LEFT) ? im_sword_left : im_sword_right));
}
