#include "common.h"
#include "sfxeng.h"
#include "objects_common.h"
//this shouldn't be here
#include "players_common.h"
#include "key.h"


Key::Key(string imagename, Uint16 xcord, Uint16 ycord, string iname):
  Item(imagename,xcord,ycord,iname),
  au_key(NULL) { }
Key::~Key() { }

bool Key::act(Object* obj) {
    if ((obj==NULL) || (obj!=owner)) return false;
    std::set<Object *> enter=owner->getEnter();
    object_iterator i=enter.begin();
    while (i!=enter.end()) {
        if ((*i)->act(this)) {
            sfxeng->playWAV(au_key);
            owner->removeItem();
            return true;
        } else ++i;
    }
    return false;
}
