#include "common.h"
#include "objects_common.h"
//shouldn't be here...
#include "characters_common.h"
#include "water.h"


Water::Water(string imagename, Uint16 xcord, Uint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname) { }
Water::~Water() { }

void Water::enter(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->setState(STATE_WATER);
    }
}

void Water::leave(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->unsetState(STATE_WATER);
    }
}
