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
  otype(OTYPE_COMMON),
  delete_flag(false) {
    onum=++scenario->max_obj_num;

    /* Parameters */
    if (hasParam(parameters,"anim")) {
        ParameterMap anim_param=getParameters(parameters["anim"],':');
        anim_orig=loadAnimation(anim_param);
    } else if (hasParam(parameters,"image")) {
        ParameterMap anim_param;
        anim_param["image"]=parameters["image"];
        anim_param["image_hor_frames"]="1";
        anim_orig=loadAnimation(anim_param);
    } else anim_orig=EmptyAnimationPtr(new EmptyAnimation());
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
    animation=anim_orig;
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

EmptyAnimationPtr Object::loadAnimation(const ParameterMap& parameters) {
    double scale_factor=1;
    BasePointType bp_type=BP_MD;
    Uint16 animation_type=ATYPE_LOOP;
    double fps=0;
    AllignType allign_type=AT_MD;
    
    if (hasParam(parameters,"scale")) {
        if (parameters.find("scale")->second == "lvl") scale_factor=config.lvlscale;
        else scale_factor=atof(parameters.find("scale")->second.c_str());
    }
    if (hasParam(parameters,"bp_type")) {
        if        (parameters.find("bp_type")->second.find("hleft")   !=string::npos) {
            if      (parameters.find("bp_type")->second.find("vup")     !=string::npos) bp_type=BP_LU;
            else if (parameters.find("bp_type")->second.find("vmiddle") !=string::npos) bp_type=BP_LM;
            else if (parameters.find("bp_type")->second.find("vdown")   !=string::npos) bp_type=BP_LD;
        } else if (parameters.find("bp_type")->second.find("hmiddle") !=string::npos) {
            if      (parameters.find("bp_type")->second.find("vup")     !=string::npos) bp_type=BP_MU;
            else if (parameters.find("bp_type")->second.find("vmiddle") !=string::npos) bp_type=BP_MM;
            else if (parameters.find("bp_type")->second.find("vdown")   !=string::npos) bp_type=BP_MD;
        } else if (parameters.find("bp_type")->second.find("hright")  !=string::npos) {
            if      (parameters.find("bp_type")->second.find("vup")     !=string::npos) bp_type=BP_RU;
            else if (parameters.find("bp_type")->second.find("vmiddle") !=string::npos) bp_type=BP_RM;
            else if (parameters.find("bp_type")->second.find("vdown")   !=string::npos) bp_type=BP_RD;
        }
    }
    if (hasParam(parameters,"type")) {
        animation_type=NOTHING;
        if (parameters.find("type")->second.find("once")    !=string::npos)  animation_type|=ATYPE_ONCE;
        if (parameters.find("type")->second.find("loop")    !=string::npos)  animation_type|=ATYPE_LOOP;
        if (parameters.find("type")->second.find("swing")   !=string::npos)  animation_type|=ATYPE_SWING;
        if (parameters.find("type")->second.find("step")    !=string::npos)  animation_type|=ATYPE_STEP;
        if (parameters.find("type")->second.find("reverse") !=string::npos)  animation_type<<=1;

        if (animation_type==NOTHING) animation_type=ATYPE_LOOP;
        if (parameters.find("type")->second.find("switch")) animation_type|=ATYPE_ST_SWITCH;
    }
    if (hasParam(parameters,"fps")) fps=atof(parameters.find("fps")->second.c_str());
    if (hasParam(parameters,"allign_type")) {
        if        (parameters.find("allign_type")->second.find("hleft")   !=string::npos) {
            if      (parameters.find("allign_type")->second.find("vup")     !=string::npos) allign_type=AT_LU;
            else if (parameters.find("allign_type")->second.find("vmiddle") !=string::npos) allign_type=AT_LM;
            else if (parameters.find("allign_type")->second.find("vdown")   !=string::npos) allign_type=AT_LD;
        } else if (parameters.find("allign_type")->second.find("hmiddle") !=string::npos) {
            if      (parameters.find("allign_type")->second.find("vup")     !=string::npos) allign_type=AT_MU;
            else if (parameters.find("allign_type")->second.find("vmiddle") !=string::npos) allign_type=AT_MM;
            else if (parameters.find("allign_type")->second.find("vdown")   !=string::npos) allign_type=AT_MD;
        } else if (parameters.find("allign_type")->second.find("hright")  !=string::npos) {
            if      (parameters.find("allign_type")->second.find("vup")     !=string::npos) allign_type=AT_RU;
            else if (parameters.find("allign_type")->second.find("vmiddle") !=string::npos) allign_type=AT_RM;
            else if (parameters.find("allign_type")->second.find("vdown")   !=string::npos) allign_type=AT_RD;
        }
    }

    /* Use animation name */
    if (hasParam(parameters,"name")) {
        string anim_name=parameters.find("name")->second;
        return loadAnimation(anim_name,scale_factor,bp_type,animation_type,fps,allign_type);

    /* Use a base image */
    } else if (hasParam(parameters,"image")) {
        string image_name=parameters.find("image")->second;
        Uint16 frames=1;
        Uint16 start_pos=0;
        if (hasParam(parameters,"frames")) frames=atoi(parameters.find("frames")->second.c_str());
        if (hasParam(parameters,"start_pos")) start_pos=atoi(parameters.find("start_pos")->second.c_str());

        /* threat as horizontal animation (constant width, height) with <image_hor_frames> frames */
        if (hasParam(parameters,"image_hor_frames")) {
            Uint16 image_hor_frames=atoi(parameters.find("image_hor_frames")->second.c_str());
            return loadAnimation(scenario->imgcache->loadImage(image_hor_frames,image_name,scale_factor),frames,bp_type,animation_type,fps,start_pos,allign_type);

        /* threat as horizontal animation (constant width, height) with width <image_hor_width> and an optional shift */
        } else if (hasParam(parameters,"image_hor_width")) {
            Uint16 image_hor_width=atoi(parameters.find("image_hor_width")->second.c_str());
            Uint16 image_hor_shift=0;
            if (hasParam(parameters,"image_hor_shift")) image_hor_shift=atoi(parameters.find("image_hor_shift")->second.c_str());
            return loadAnimation(scenario->imgcache->loadImage(image_hor_width,image_hor_shift,image_name,scale_factor),frames,bp_type,animation_type,fps,start_pos,allign_type);

        /* threat as constant width/height animation based on a base rectangle */
        } else if (hasParam(parameters,"image_rect")) {
            ParameterMap rect_param=getParameters(parameters.find("image_rect")->second,';');
            SDL_Rect rect;
            rect.x=rect.y=0;
            rect.w=rect.h=10;
            if (hasParam(rect_param,"x")) rect.x=atoi(rect_param.find("x")->second.c_str());
            if (hasParam(rect_param,"y")) rect.y=atoi(rect_param.find("y")->second.c_str());
            if (hasParam(rect_param,"w")) rect.w=atoi(rect_param.find("w")->second.c_str());
            if (hasParam(rect_param,"h")) rect.h=atoi(rect_param.find("h")->second.c_str());
            return loadAnimation(scenario->imgcache->loadImage(rect,image_name,scale_factor),frames,bp_type,animation_type,fps,start_pos,allign_type);

        /* get informations from an image description file */
        } else {
            string image_desc=image_name+".dsc";
            if (hasParam(parameters,"image_desc")) image_desc=parameters.find("image_desc")->second;
            return loadAnimation(scenario->imgcache->loadImage(image_name,scale_factor,image_desc),frames,bp_type,animation_type,fps,start_pos,allign_type);
        }

    /* no suitable animation found */
    } else {
        cout << "Invalid animation parameters!" << endl;
        return EmptyAnimationPtr(new EmptyAnimation());
    }
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

    /* TODO: support surface creation and use it as a fallback if no image was specified */
}
Background::~Background() { }
