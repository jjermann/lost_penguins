#include "lost_vikings.h"
#include "objects.h"
#include "events.h"

using namespace std;

Object::Object(string imagename, Uint16 xcord, Uint16 ycord, string oname) {
    im_orig=new Animation(imgcache->loadImage(imagename));
    animation=im_orig;
    pos.x=xcord;
    pos.y=ycord;
    pos.w=animation->getWidth();
    pos.h=animation->getHeight();
    curpos.w=pos.w;
    curpos.h=pos.h;
    curpos.x=0;
    curpos.y=0;
    name=oname;
    otype=NOTHING;
    delete_flag=false;
    event=NULL;
}

Object::~Object() {
    if (event) event->cancel();
    if (!(otype&OTYPE_CHARACTER)) delete animation;
}

bool Object::isIn(const SDL_Rect& rect, bool touch, Uint16 radius) {
    SDL_Rect center=getCenter();
    if (touch) {
    if ((pos.x+pos.w+radius) > rect.x && pos.x < (rect.x+rect.w+radius) && (pos.y+pos.h+radius) > rect.y && pos.y < (rect.y+rect.h+radius)) return true;
    else return false;
    } else {
    if ((center.x+radius)>rect.x && center.x<(rect.x+rect.w+radius) && (center.y+radius)>rect.y && center.y<(rect.y+rect.h+radius)) return true;
    else return false;
    }
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


/*==================
    Item (Object)
==================*/

Item::Item(string imagename, Uint16 xcord, Uint16 ycord, string iname):
  Object(imagename,xcord,ycord,iname), owner(NULL) {
    otype|=OTYPE_ITEM;
}
Item::~Item() { }
