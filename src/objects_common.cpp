#include "common.h"
#include "imgcache.h"
#include "anim.h"
#include "events.h"
#include "objects_common.h"


Object::Object(string imagename, Sint16 xcord, Sint16 ycord, string oname):
  tbirth(SDL_GetTicks()),
  state(NOTHING),
  event(NULL),
  im_orig(new Animation(imgcache->loadImage(imagename))),
  otype(NOTHING),
  name(oname),
  delete_flag(false) {
    animation=im_orig;
    pos.x=xcord;
    pos.y=ycord;
    pos.w=animation->getWidth();
    pos.h=animation->getHeight();
    curpos.w=pos.w;
    curpos.h=pos.h;
    curpos.x=0;
    curpos.y=0;
}

Object::~Object() {
    if (event) event->cancel();
    if (!(otype&OTYPE_CHARACTER)) delete animation;
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
    //Did we hit a maparea?
    if (xcord < 0) {
        pos.x=0;
        ok=false;
    } else if (xcord > (maparea->w-pos.w)) {
        pos.x=maparea->w-pos.w;
        ok=false;
    } else pos.x=xcord;
    if (ycord>(maparea->h-pos.h)) {
        pos.y=maparea->h-pos.h;
        ok=false;
    } else pos.y=ycord;
    return ok;
}

const Frame& Object::getFrame() const {
    return animation->getFrame();
}
bool Object::updateAnim(Uint16 dt) {
    return (animation->updateAnim(dt));
}
void Object::setAnim(Animation* anim) {
    animation=anim;
    animation->start();
}
void Object::resetAnimState() {
    animation=im_orig;
    animation->setFrame(0);
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
