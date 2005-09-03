#include "common.h"
#include "imgcache.h"
#include "animation.h"
#include "events.h"
#include "objects_common.h"
//HACK!
#include "scenario.h"


Object::Object(string imagename, Sint16 xcord, Sint16 ycord, string oname):
  state(NOTHING),
  event(NULL),
  im_orig(loadAnimation(scenario->imgcache->loadImage(1,imagename))),
  otype(NOTHING),
  name(oname),
  delete_flag(false) {
    animation=im_orig;
    pos=animation->getFrameDim();
    pos.x=xcord;
    pos.y=ycord;
    onum=++scenario->max_obj_num;
}

Object::~Object() {
    if (event) event->cancel();
    if (!(otype&OTYPE_CHARACTER)) delete animation;
}

bool Object::operator<(const Object& obj) const {
    if (onum<obj.onum) return true;
    else return false;
}

bool Object::isIn(const SDL_Rect& rect, bool touch) {
    if (touch) {
        if ((pos.x+pos.w) > rect.x && pos.x < (rect.x+rect.w) && (pos.y+pos.h) > rect.y && pos.y < (rect.y+rect.h)) return true;
        else return false;
    } else {
        SDL_Rect center=getCenter();
        if (center.x>rect.x && center.x<(rect.x+rect.w) && center.y>rect.y && center.y<(rect.y+rect.h)) return true;
        else return false;
    }
}

bool Object::setPos(Sint16 xcord,Sint16 ycord) {
    bool ok=true;
    //Did we hit a map area?
    if (xcord < 0) {
        pos.x=0;
        ok=false;
    } else if (xcord > (scenario->area->w-pos.w)) {
        pos.x=scenario->area->w-pos.w;
        ok=false;
    } else pos.x=xcord;
    if (ycord>(scenario->area->h-pos.h)) {
        pos.y=scenario->area->h-pos.h;
        ok=false;
    } else pos.y=ycord;
    return ok;
}

Animation* Object::loadAnimation(const Image& abase_image,
                         Uint32 aduration,
                         Uint16 aframes,
                         Uint16 aanimation_type,
                         Uint16 astart_pos,
                         BasePointType abp_type, 
                         AllignType aallign_type,
                         Sint16 ashift_x, 
                         Sint16 ashift_y) {
    Animation* anim=new Animation(abase_image,aduration,aframes,aanimation_type,astart_pos,abp_type,aallign_type,ashift_x,ashift_y);
    anim->setBasePos(&pos);
    return anim;
}

SDL_Rect Object::getDrawPos() const {
    return animation->getDrawPos();
}
const Frame Object::getFrame() const {
    return animation->getFrame();
}
bool Object::updateAnim(Uint16 dt) {
    return (animation->updateAnim(dt));
}
void Object::setAnim(Animation* anim) {
    animation=anim;
    animation->setBasePos(&pos);
    animation->runAnim();
}
void Object::resetAnimState() {
    animation=im_orig;
    animation->setBasePos(&pos);
    animation->runAnim();
}
bool Object::isRunning() const {
    return animation->isRunning();
}

//Events (triggered animations/effects)
void Object::clearEvents() {
    event=NULL;
}
void Object::setEvent(Event* ev) {
    if (ev==NULL) return;
    if (event) event->cancel();
    event=ev;
    updateEvents(0);
}
Uint16 Object::updateEvents(Uint16 dt) {
    Uint16 evstate=NOTHING;
    if (event) evstate=event->update(dt);
    switch (evstate) {
        case EV_CANCEL: {
            event->cancel();
            break;
        }
        case EV_START: {
            event->start();
            break;
        }
        case EV_END: {
            event->end();
            break;
        }
        default: { }
    }
    return evstate;
}
void Object::cancelEvent() {
    if (event) event->cancel();
}
void Object::stopEvent() {
    if (event) event->end();
}


Item::Item(string imagename, Sint16 xcord, Sint16 ycord, string iname):
  Object(imagename,xcord,ycord,iname), owner(NULL) {
    otype|=OTYPE_ITEM;
}
Item::~Item() { }


Background::Background(string imagename):
  Object(imagename,0,0,"Background") {
    otype=NOTHING;
}
Background::~Background() { }
