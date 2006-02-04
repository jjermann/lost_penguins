#include "common.h"
//shouldn't be here...
#include "objectpools.h"
#include "scenario.h"
#include "players_common.h"
#include "imgcache.h"
#include "triggered_bomb.h"
#include "bomb.h"

Bomb::Bomb(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Item(xcord,ycord,param) {
    /* Parameters */
    if (!hasParam(parameters,"image")) anim_orig=loadAnimation(scenario->imgcache->loadImage(1,"bomb_fire.bmp"));
    if (hasParam(parameters,"image_bomb")) triggered_bomb_param["image"]=parameters["image_bomb"];
      else triggered_bomb_param["image"]="bomb_fire.bmp";
    if (hasParam(parameters,"countdown"))  triggered_bomb_param["countdown"]="3000";
      else triggered_bomb_param["countdown"]=parameters["countdown"];
    if (hasParam(parameters,"audio_bomb")) triggered_bomb_param["audio_bomb"]=parameters["audio_bomb"];
      else triggered_bomb_param["audio_bomb"]="explsn.wav";
}
Bomb::~Bomb() { }

bool Bomb::act(Object*) {
    if (scenario->pool->addObjectbyName("TriggeredBomb",owner->getPos()->x,owner->getPos()->y-20,triggered_bomb_param)) {
        owner->removeItem();
        return true;
    } else {
        return false;
    }
}

