#include "common.h"
#include "imgcache.h"
#include "animation.h"
#include "events.h"
#include "objects_common.h"
//HACK!
#include "scenario.h"


Object::Object(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  state(NOTHING),
  event(NULL),
  otype(NOTHING),
  delete_flag(false) {
    onum=++scenario->max_obj_num;
    animation.reset(new EmptyAnimation(&anim_orig));

    /* Parameters */
    anim_orig=loadAnimation(parameters);
    if (hasParam(parameters,"name")) name=parameters["name"];
      else name="Object";
    if (hasParam(parameters,"w")) pos.w=atoi(parameters["w"].c_str());
      else if (anim_orig->isValid()) pos.w=anim_orig->getFrameDim().w;
      else pos.w=(Uint16)(DATA_LVLPLAYER_W*sqrt(config.lvlscale>=0 ? config.lvlscale : -1/config.lvlscale)+0.5);
    if (hasParam(parameters,"h")) pos.h=atoi(parameters["h"].c_str());
      else if (anim_orig->isValid()) pos.h=anim_orig->getFrameDim().h;
      else pos.h=(Uint16)(DATA_LVLPLAYER_H*sqrt(config.lvlscale>=0 ? config.lvlscale : -1/config.lvlscale)+0.5);
    pos.x=xcord;
    pos.y=ycord;
}

Object::~Object() {
    if (event) event->cancel();
}

bool Object::operator<(const Object& obj) const {
    if (onum<obj.onum) return true;
    else return false;
}

bool Object::isIn(const SDL_Rect& rect, bool touch) const {
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

EmptyAnimationPtr Object::loadAnimation(string anim_name,
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
        return EmptyAnimationPtr(new EmptyAnimation());
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
    return EmptyAnimationPtr(new EmptyAnimation());
}

EmptyAnimationPtr Object::loadAnimation(const Image& abase_image,
                                        Uint16 aframes,
                                        BasePointType abp_type, 
                                        Uint16 aanimation_type,
                                        double afps,
                                        Uint16 astart_pos,
                                        AllignType aallign_type) {
    EmptyAnimationPtr anim(new Animation(abase_image,aframes,abp_type,aanimation_type,afps,astart_pos,aallign_type));
    anim->setBasePos(&pos);
    return anim;
}

EmptyAnimationPtr Object::loadAnimation(ParameterMap& parameters) {
    if (hasParam(parameters,"image")) return loadAnimation(scenario->imgcache->loadImage(1,parameters["image"]));
        return EmptyAnimationPtr(new EmptyAnimation());
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
bool Object::setAnim(EmptyAnimationPtr anim, bool start) {
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


Item::Item(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Object(xcord,ycord,parameters), owner(NULL) {
    otype|=OTYPE_ITEM;
}
Item::~Item() { }


Background::Background(ParameterMap& parameters):
  Object(0,0,parameters) {
    otype=NOTHING;
    name="Background";

    /* Parameters */
    /* TODO: support surface creation and use it as a fallback if no image was specified */
    anim_orig=loadAnimation(parameters);
    pos=anim_orig->getFrameDim();
}
Background::~Background() { }
