#include "common.h"
#include "objectpools.h"
//shouldn't be here...
#include "scenario.h"
#include "imgcache.h"
#include "players_common.h"
#include "exit.h"


Exit::Exit(Sint16 xcord, Sint16 ycord, const ParameterMap& param):
  Object(xcord,ycord,param) {
    /* Parameters */
    if (!hasParam(parameters,"image")) anim_orig=loadAnimation(scenario->imgcache->loadImage(1,"exit.bmp"));
}
Exit::~Exit() { }

bool Exit::act(Object*) {
    player_iterator it=scenario->pool->playerspool.begin();
    while (it!=scenario->pool->playerspool.end()) {
        if (!((*it)->isIn(pos))) return false;
        ++it;
    }
    scenario->finnished=true;
    return true;
}
