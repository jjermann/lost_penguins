#include "common.h"
#include "weapons.h"
#include "objects_common.h"
//shouldn't be here...
#include "vikings_common.h"
#include "heart.h"


Heart::Heart(string imagename, Uint16 xcord, Uint16 ycord, string iname):
  Item(imagename,xcord,ycord,iname) { }
Heart::~Heart() { }

bool Heart::act(Object* obj) {
    if ((obj==NULL) || (obj!=owner)) return false;
    Weapon tmpheal(1,W_HEAL,WS_HEAL);
    owner->hit(DIR_ALL,tmpheal);
    owner->removeItem();
    return true;
}
