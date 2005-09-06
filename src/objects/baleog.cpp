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
    anim_left=loadAnimation(scenario->imgcache->loadImage(1,"baleog1_left.bmp"));
    anim_right=loadAnimation(scenario->imgcache->loadImage(1,"baleog1_right.bmp"));
    anim_walk_left=loadAnimation(scenario->imgcache->loadImage("baleog1-run_left.png"),8);
    anim_walk_right=loadAnimation(scenario->imgcache->loadImage(8,"baleog1-run_right.png"),8);
    anim_crash_left=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_left.bmp"),1,BP_LD,ATYPE_ONCE_END,calcFPS(1,T_IRR));
    anim_crash_right=loadAnimation(scenario->imgcache->loadImage(1,"olaf1_land_right.bmp"),1,BP_RD,ATYPE_ONCE_END,calcFPS(1,T_IRR));
    weapon=Weapon(-1,W_STRIKE);
    anim_sword_left=loadAnimation(scenario->imgcache->loadImage(8,"BaleogCyborg_Slash_left.png"),8,BP_MD,ATYPE_ONCE_END);
    anim_sword_right=loadAnimation(scenario->imgcache->loadImage(8,"BaleogCyborg_Slash_right.png"),8,BP_MD,ATYPE_ONCE_END);
    au_sword=scenario->sndcache->loadWAV("swrdsw2.wav");
}

Baleog::~Baleog() { }

//Baleog1: Sword attack
void Baleog::in_sp1() {
    setEvent(new EAttack(this,10,&weapon,(state&STATE_LEFT) ? DIR_LEFT : DIR_RIGHT,10,enemy_types,0,0,au_sword,(state&STATE_LEFT) ? anim_sword_left : anim_sword_right));
}
