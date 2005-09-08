#include "common.h"
#include "objectpools.h"
#include "scenario.h"
#include "objects_common.h"
#include "imgcache.h"
#include "trigger.h"


Trigger::Trigger(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Object(xcord,ycord,parameters) {
    /* Parameters */
    if (!hasParam(parameters,"image")) anim_orig=loadAnimation(scenario->imgcache->loadImage(1,"key.bmp"));
    if (hasParam(parameters,"key")) key=parameters["key"];
      else key="Key";
    if (hasParam(parameters,"target")) target=scenario->pool->getObject(parameters["target"]);
      else target=scenario->pool->getObject("Target");
}
Trigger::~Trigger() { }

bool Trigger::act(Object* obj) {
    //sanity check
    if (!(key.empty() || (obj && obj->getName()==key))) return false;

    if (target) {
        if (target->act(this)) return true;
        else return false;
    } else return false;
}
