#include "common.h"
#include "events.h"
#include "animation.h"
#include "imgcache.h"
#include "sndcache.h"
#include "input.h"
#include "scenario.h"
#include "baleog.h"


Baleog::Baleog(string imagename, Sint16 xcord, Sint16 ycord, string pname):
  Player(imagename,xcord,ycord,pname) {
    im_left=loadAnimation(scenario->imgcache->loadImage(1,"baleog1_left.bmp"));
    im_right=loadAnimation(scenario->imgcache->loadImage(1,"baleog1_right.bmp"));
    im_run_left=loadAnimation(scenario->imgcache->loadImage(8,"baleog1-run_left.png"),1000,8);
    im_run_right=loadAnimation(scenario->imgcache->loadImage(8,"baleog1-run_right.png"),1000,8);
    im_fall_left=im_left;
    im_fall_right=im_right;
    im_krit_left=im_left;
    im_krit_right=im_right;
    im_land_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),T_IRR,1,ATYPE_ONCE_END);
    im_land_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),T_IRR,1,ATYPE_ONCE_END);
    weapon=Weapon(-1,W_STRIKE);
    im_sword_left=loadAnimation(scenario->imgcache->loadImage(8,"BaleogCyborg_Slash_left.png"),1000,8,ATYPE_ONCE_END);
    im_sword_right=loadAnimation(scenario->imgcache->loadImage(8,"BaleogCyborg_Slash_right.png"),1000,8,ATYPE_ONCE_END);
    au_sword=scenario->sndcache->loadWAV("swrdsw2.wav");
}

Baleog::~Baleog() {
    delete im_left;
    delete im_right;
    delete im_run_left;
    delete im_run_right;
    delete im_land_left;
    delete im_land_right;
    delete im_sword_left;
    delete im_sword_right;
}

//Baleog1: Sword attack
void Baleog::in_sp1() {
    setEvent(new EAttack(this,10,&weapon,(state&STATE_LEFT) ? DIR_LEFT : DIR_RIGHT,10,enemy_types,0,0,au_sword,(state&STATE_LEFT) ? im_sword_left : im_sword_right));
}
