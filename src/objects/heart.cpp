#include "common.h"
#include "weapons.h"
#include "objects_common.h"
//shouldn't be here...
#include "scenario.h"
#include "imgcache.h"
#include "players_common.h"
#include "heart.h"


Heart::Heart(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Item(xcord,ycord,parameters) {
    /* Parameters */
    if (!hasParam(parameters,"image")) anim_orig=loadAnimation(scenario->imgcache->loadImage(1,"heart.bmp"));
    if (hasParam(parameters,"alife")) alife=atoi(parameters["alife"].c_str());
      else alife=1;
}
Heart::~Heart() { }

bool Heart::act(Object* obj) {
    if ((obj==NULL) || (obj!=owner)) return false;
    Weapon tmpheal(alife,W_HEAL,WS_HEAL);
    owner->hit(DIR_ALL,tmpheal);
    owner->removeItem();
    return true;
}
