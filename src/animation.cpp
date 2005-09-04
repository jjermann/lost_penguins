#include "common.h"
#include "animation.h"

Animation::Animation(const Image& abase_image,
          Uint16 aframes,
          Uint16 aanimation_type,
          double afps,
          Uint16 astart_pos,
          BasePointType abp_type,
          AllignType aallign_type,
          Sint16 ashift_x,
          Sint16 ashift_y):
  base_image(abase_image),
  frames(aframes),
  animation_type(aanimation_type),
  fps(afps),
  start_pos(astart_pos),
  bp_type(abp_type),
  allign_type(aallign_type),
  shift_x(ashift_x),
  shift_y(ashift_y),
  end_pos(start_pos+frames),
  base_pos(NULL) {
    if (fps==0) {
        fps=calcFPS(frames,DATA_LVLDUR);
    } else if (fps<0) {
        fps=calcFPS(frames,(Uint32)(-fps));
    }
    duration=(Uint32)(frames*1000.0/fps+0.5);
    checkAnim();
}
Animation::~Animation() { }
void Animation::runAnim() {
    if (!isValid()) {
        cout << "Starting an invalid animation!" << endl;
        return;
    }
    is_running=true;
    cur_time=0;
    if (animation_type&ATYPE_ALL_NORMAL) {
        forward=true;
        cur_frame_num=0;
    } else {
        forward=false;
        cur_frame_num=frames-1;
    }
}
bool Animation::stopAnim() {
    if (!isValid()) {
        cout << "Stopping an invalid animation!" << endl;
        return is_running=false;
    } else if (animation_type&ATYPE_ALL_ONCE) {
        is_running=false;
        cur_time=0;
        if (animation_type&ATYPE_ALL_LSTART) {
            cur_frame_num=0;
        } else {
            cur_frame_num=frames-1;
        }
    }
    return isRunning();
}

bool Animation::updateAnim(Uint16 dt) {
    if (!isValid()) {
        cout << "Updating an invalid animation!" << endl;
        return false;
    }
    if (!isRunning()) return false;
    cur_time+=dt;
    Uint32 durationh=(Uint32)duration/2;

    if        (animation_type&ATYPE_ONCE || animation_type&ATYPE_ONCE_END) {
        if (cur_time>=duration) {
            return stopAnim();
        } else {
            cur_frame_num=(Uint16)(cur_time*frames/duration);
        }
    } else if (animation_type&ATYPE_LOOP) {
        if (cur_time>=duration) {
            while(cur_time>=duration) cur_time-=duration;
        }
        cur_frame_num=(Uint16)(cur_time*frames/duration);
    } else if (animation_type&ATYPE_ONCE_REV || animation_type&ATYPE_ONCE_END_REV) {
        if (cur_time>=duration) {
            return stopAnim();
        } else {
            cur_frame_num=(Uint16)((duration-cur_time-1)*frames/duration);
        }
    } else if (animation_type&ATYPE_LOOP_REV) {
        if (cur_time>=duration) {
            while(cur_time>=duration) cur_time-=duration;
        }
        cur_frame_num=(Uint16)((duration-cur_time-1)*frames/duration);
    } else if (animation_type&ATYPE_SWING && forward) {
        if (cur_time>=durationh) {
            forward=false;
            cur_frame_num=(Uint16)((duration-cur_time-1)*frames/durationh);
        } else {
            cur_frame_num=(Uint16)(cur_time*frames/durationh);
        }
    } else if (animation_type&ATYPE_SWING) {
        if (cur_time>=duration) {
            while(cur_time>=duration) cur_time-=duration;
            forward=true;
            cur_frame_num=(Uint16)(cur_time*frames/durationh);
        } else {
            cur_frame_num=(Uint16)((duration-cur_time-1)*frames/durationh);
        }
    } else if (animation_type&ATYPE_SWING_REV && forward) {
        if (cur_time>=duration) {
            while(cur_time>=duration) cur_time-=duration;
            forward=false;
            cur_frame_num=(Uint16)((duration-cur_time-1)*frames/durationh);
        } else {
            cur_frame_num=(Uint16)(cur_time*frames/durationh);
        }
    } else if (animation_type&ATYPE_SWING_REV) {
        if (cur_time>=durationh) {
            forward=true;
            cur_frame_num=(Uint16)(cur_time*frames/durationh);
        } else {
            cur_frame_num=(Uint16)((duration-cur_time-1)*frames/durationh);
        }
    } else {
        cout << "This shouldn't happen: Unknown animation_type!" << endl;
        return stopAnim();
    }
    return isRunning();
}
Frame Animation::getFrame() const {
    if (!isValid()) cout << "Invalid Frame of an invalid animation!" << endl;
    return Frame(base_image.surface,base_image.description[cur_frame_num+start_pos]);
}
Frame Animation::getBaseFrame() const {
    if (!isValid()) cout << "Invalid Base Frame of an invalid animation!" << endl;
    Uint16 num;
    if (animation_type&ATYPE_ALL_NORMAL) num=start_pos;
    else num=end_pos;
    return Frame(base_image.surface,base_image.description[num]);
}
SDL_Rect Animation::getDrawPos() const {
    SDL_Rect draw_pos=base_image.description[cur_frame_num];
    if (!base_pos || !isValid()) {
        if (!isValid()) cout << "Invalid DrawPos of an invalid animation!" << endl;
        else cout << "Invalid DrawPos!" << endl;
        draw_pos.x=draw_pos.y=draw_pos.w=draw_pos.h=0;
        return draw_pos;
    }
    draw_pos.x=base_pos->x;
    draw_pos.y=base_pos->y;

    /* calculate the base point */
    Sint16 base_point_x=0;
    Sint16 base_point_y=0;

    /* BP_MD is used most times */
    if        (bp_type==BP_MD) {
        base_point_x=(Uint16)draw_pos.w/2;
        base_point_y=draw_pos.h;
    } else if (bp_type==BP_LU) {
        base_point_x=0;
        base_point_y=0;
    } else if (bp_type==BP_LM) {
        base_point_x=0;
        base_point_y=(Uint16)draw_pos.h/2;
    } else if (bp_type==BP_LD) {
        base_point_x=0;
        base_point_y=draw_pos.h;
    } else if (bp_type==BP_MU) {
        base_point_x=(Uint16)draw_pos.w/2;
        base_point_y=0;
    } else if (bp_type==BP_MM) {
        base_point_x=(Uint16)draw_pos.w/2;
        base_point_y=(Uint16)draw_pos.h/2;
    } else if (bp_type==BP_RU) {
        base_point_x=draw_pos.w;
        base_point_y=0;
    } else if (bp_type==BP_RM) {
        base_point_x=draw_pos.w;
        base_point_y=(Uint16)draw_pos.h/2;
    } else if (bp_type==BP_RD) {
        base_point_x=draw_pos.w;
        base_point_y=draw_pos.h;
    } else {
        cout << "This shouldn't happen: Unknown base point type!" << endl;
        base_point_x=(Uint16)draw_pos.w/2;
        base_point_y=draw_pos.h;
    }

    /* calculate the draw_pos using the allignment type */
    if        (allign_type==AT_MD) {
        draw_pos.x+=(Uint16)base_pos->w/2-base_point_x;
        draw_pos.y+=base_pos->h-base_point_y;
    } else if (allign_type==AT_LU) {
        draw_pos.x-=base_point_x;
        draw_pos.y-=base_point_y;
    } else if (allign_type==AT_LM) {
        draw_pos.x-=base_point_x;
        draw_pos.y+=(Uint16)base_pos->h/2-base_point_y;
    } else if (allign_type==AT_LD) {
        draw_pos.x-=base_point_x;
        draw_pos.y+=base_pos->h-base_point_y;
    } else if (allign_type==AT_MU) {
        draw_pos.x+=(Uint16)base_pos->w/2-base_point_x;
        draw_pos.y-=base_point_y;
    } else if (allign_type==AT_MM) {
        draw_pos.x+=(Uint16)base_pos->w/2-base_point_x;
        draw_pos.y+=(Uint16)base_pos->h/2-base_point_y;
    } else if (allign_type==AT_RU) {
        draw_pos.x+=base_pos->w-base_point_x;
        draw_pos.y-=base_point_y;
    } else if (allign_type==AT_RM) {
        draw_pos.x+=base_pos->w-base_point_x;
        draw_pos.y+=(Uint16)base_pos->h/2-base_point_y;
    } else if (allign_type==AT_RD) {
        draw_pos.x+=base_pos->w-base_point_x;
        draw_pos.y+=base_pos->h-base_point_y;
    } else {
        cout << "This shouldn't happen: Unknown allign type!" << endl;
        draw_pos.x+=(Uint16)base_pos->w/2-base_point_x;
        draw_pos.y+=base_pos->h-base_point_y;
    }

    /* substract shift ("add" it to the bp position) */
    draw_pos.x-=shift_x;
    draw_pos.y-=shift_y;

    return draw_pos;
}

SDL_Rect Animation::getFrameDim() const {
    if (!isFixed()) cout << "Invalid Frame dimension of an invalid animation!" << endl;
    SDL_Rect rect;
    rect.x=rect.y=0;
    rect.w=base_image.description[start_pos].w;
    rect.h=base_image.description[start_pos].h;
    return rect;
}

bool Animation::checkAnim() {
    if (base_image.surface==NULL || base_image.description.empty()) {
        is_valid=false;
    } else if (base_image.description.size()==1 || duration==0 || frames==1) {
        is_image=true;
        cur_time=0;
        forward=true;
        cur_frame_num=0;
        is_running=false;
        is_fixed=true;
        is_valid=true;
    } else if (frames==0 || start_pos>end_pos || (start_pos+frames)>base_image.description.size()) {
        is_valid=false;
    } else {
        is_valid=true;
        is_image=false;
        is_fixed=false;
        if (animation_type&ATYPE_ALL_ONCE) is_running=false;
        else is_running=true;
        cur_time=0;
        if (animation_type&ATYPE_ALL_NORMAL) {
            forward=true;
            cur_frame_num=0;
        } else {
            forward=false;
            cur_frame_num=frames-1;
        }
    }
    return is_valid;
}
