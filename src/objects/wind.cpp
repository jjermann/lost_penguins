#include "common.h"
#include "objects_common.h"
//shouldn't be here...
#include "characters_common.h"
#include "wind.h"

Wind::Wind(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Object(xcord,ycord,parameters) {
    /* Parameters */
    if (hasParam(parameters,"accel")) gravitymod=atoi(parameters["accel"].c_str());
      else gravitymod=0;
}
Wind::~Wind() { }

void Wind::enter(Object *obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->addGravity(gravitymod);
    }
}
void Wind::leave(Object *obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->addGravity(-gravitymod);
    }
}
