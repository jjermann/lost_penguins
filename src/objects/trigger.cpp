#include "common.h"
#include "objectpools.h"
#include "objects_common.h"
#include "trigger.h"


Trigger::Trigger(string imagename, Uint16 xcord, Uint16 ycord, string targetname, string oname, string keyname):
  Object(imagename,xcord,ycord,oname) {
    target=pool->getObject(targetname);
    key=keyname;
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
