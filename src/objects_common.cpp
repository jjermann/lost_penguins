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
  otype(NOTHING),
  name(oname),
  delete_flag(false) {
    anim_orig=loadAnimation(scenario->imgcache->loadImage(1,imagename));
    animation=anim_orig;
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

EmptyAnimation* Object::loadAnimation(string anim_name,
                         double scale_factor,
                         BasePointType abp_type,
                         Uint16 aanimation_type,
                         double afps,
                         AllignType aallign_type) {

    /* Parse animation data file */
    ifstream file;
    string tmpline;
    string loadfile=config.datadir+config.anim_file;

    file.open(loadfile.c_str());
    if (!file) {
        cout << "Failed to open the animation data file: " << loadfile << " => Couldn't load " << anim_name << " animation!\n" << endl;
        return new EmptyAnimation();
    } else {
        string arg1,arg2,arg3,arg4,arg5,arg6;
        string tmp_anim_name="";
        string imagename="";
        Uint16 astart_pos=0;
        Uint16 aframes=1;

        Uint16 description_type=DESC_NONE;

        while (getline(file,tmpline)) {
            arg1=arg2=arg3=arg4=arg5=arg6="";
            std::istringstream tmpstream(tmpline);
            tmpstream >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> arg6;

            if        (arg1 == "DESCRIPTION") {
                description_type=DESC_NONE;  
            } else if (arg1 == "ANIMATION") {
                if (arg2 == "LVLANIM") {
                    description_type=DESC_ANIM_LVLANIM;
                } else {
                    description_type=DESC_NONE;
                }
            }
     
            if (description_type==DESC_ANIM_LVLANIM) {
                if (arg1.empty() || arg2.empty() || arg3.empty() || arg4.empty()) {
                } else {
                    tmp_anim_name=arg1;
                    imagename=arg2;
                    astart_pos=(Uint16)atoi(arg3.c_str());
                    aframes=(Uint16)atoi(arg4.c_str());

                    if (anim_name==tmp_anim_name) {
                        return loadAnimation(imgcache->loadImage(imagename,scale_factor),aframes,abp_type,aanimation_type,afps,astart_pos,aallign_type);
                    }
                }
            }
        }
    }
    cout << "Animation " << anim_name << " not found!" << endl;
    return new EmptyAnimation();
}

EmptyAnimation* Object::loadAnimation(const Image& abase_image,
                         Uint16 aframes,
                         BasePointType abp_type, 
                         Uint16 aanimation_type,
                         double afps,
                         Uint16 astart_pos,
                         AllignType aallign_type) {
    EmptyAnimation* anim=new Animation(abase_image,aframes,abp_type,aanimation_type,afps,astart_pos,aallign_type);
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
bool Object::setAnim(EmptyAnimation* anim, bool start) {
    if (anim && anim->isValid()) {
        animation=anim;
        if (start) {
            animation->setBasePos(&pos);
            animation->runAnim();
        }
        return true;
    } else {
        if (anim==NULL) animation=anim_orig;
        return false;
    }
}
void Object::resetAnimState() {
    setAnim(anim_orig);
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
