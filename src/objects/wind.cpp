#include "common.h"
#include "objects_common.h"
//shouldn't be here...
#include "characters_common.h"
#include "wind.h"

Wind::Wind(string imagename, Uint16 xcord, Uint16 ycord, Sint16 Accel, string oname):
  Object(imagename,xcord,ycord,oname) {
    graplitymod=Accel;
}
Wind::~Wind() { }

void Wind::enter(Object *obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->addGraplity(graplitymod);
    }
}
void Wind::leave(Object *obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->addGraplity(-graplitymod);
    }
}
