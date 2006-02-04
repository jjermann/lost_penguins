#include "common.h"
#include "events.h"
#include "animation.h"
#include "imgcache.h"
#include "sndcache.h"
#include "input.h"
#include "scenario.h"
#include "baleog.h"


Baleog::Baleog(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Player(xcord,ycord,param) {
    weapon=Weapon(-1,W_STRIKE);

    /* Parameters */
    if (hasParam(parameters,"anim_baleog_sword_left")) anim_baleog_sword_left=loadAnimation(getParameters(parameters["anim_baleog_sword_left"],':'));
      else anim_baleog_sword_left=loadAnimation(scenario->imgcache->loadImage(8,"BaleogCyborg_Slash_left.png"),8,BP_MD,ATYPE_ONCE);
    if (hasParam(parameters,"anim_baleog_sword_right")) anim_baleog_sword_right=loadAnimation(getParameters(parameters["anim_baleog_sword_right"],':'));
      else anim_baleog_sword_right=loadAnimation(scenario->imgcache->loadImage(8,"BaleogCyborg_Slash_right.png"),8,BP_MD,ATYPE_ONCE);

    if (hasParam(parameters,"audio_sword")) au_sword=scenario->sndcache->loadWAV(parameters["audio_sword"]);
      else au_sword=scenario->sndcache->loadWAV("swrdsw2.wav");
}

Baleog::~Baleog() { }

//Baleog1: Sword attack
void Baleog::in_sp1() {
    setEvent(new EAttack(this,10,&weapon,(state&STATE_LEFT) ? DIR_LEFT : DIR_RIGHT,10,enemy_types,0,0,au_sword,(state&STATE_LEFT) ? anim_baleog_sword_left : anim_baleog_sword_right));
}
