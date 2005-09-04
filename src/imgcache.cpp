#include "common.h"
#include "SDL_rotozoom.h"
#include "imgcache.h"


ImageCache::ImageCache():
  not_found("no_file.bmp") {
}
ImageCache::~ImageCache() {
    map<pair<string,double>, Image>::iterator imgit=imgcache.begin();
    while (imgit != imgcache.end()) {
        SDL_FreeSurface((*imgit).second.surface);
        imgcache.erase(imgit);
        imgit=imgcache.begin();
    }
}

SDL_Rect& ImageCache::scaleRectangle(SDL_Rect& base_rect, double scale_factor) {
    base_rect.x=(Sint16)(base_rect.x*sqrt(scale_factor));
    base_rect.y=(Sint16)(base_rect.y*sqrt(scale_factor));
    base_rect.w=(Uint16)(base_rect.w*sqrt(scale_factor));
    base_rect.h=(Uint16)(base_rect.h*sqrt(scale_factor));
    return base_rect;
}

std::vector<SDL_Rect>& ImageCache::scaleDescription(std::vector<SDL_Rect>& description, double scale_factor) {
    for (Uint16 i=0; i<description.size(); i++) {
        scaleRectangle(description[i],scale_factor);
    }
    return description;
}

Image& ImageCache::scaleImage(Image& original_image, double scale_factor) {
    SDL_Surface* newsurface=zoomSurface(original_image.surface, sqrt(scale_factor), sqrt(scale_factor), 1);
    SDL_FreeSurface(original_image.surface);
    original_image.surface=newsurface;
    scaleDescription(original_image.description, scale_factor);
    return original_image;
}

Image& ImageCache::loadImage(std::vector<SDL_Rect> image_desc, string imagename, double scale_factor) {
    //negative values are taken as positive reciproc
    if (scale_factor<0) scale_factor=-1/scale_factor;
    //default is not_found
    if (imagename=="") {
        imagename=not_found;
        scale_factor=1;
    }
        
    map<pair<string,double>, Image>::iterator imgit=imgcache.find(make_pair(imagename,scale_factor));
    
    // Image is not yet cached
    if (imgit==imgcache.end()) {
        string loadfile=config.datadir+imagename;
        SDL_Surface* tmp_surface=NULL;
        Image return_image;
        // Loading
        #ifdef SDL_IMAGE
        if ((tmp_surface=IMG_Load(loadfile.c_str())) == NULL) {
        #else
        if ((tmp_surface=SDL_LoadBMP(loadfile.c_str())) == NULL) {
        #endif
            cout << "Couldn't load the image: " << imagename << endl;
            //Even the fallback image was not found
            if (not_found==imagename) {
                quitGame(3);
            //Try to load the fallback image...
            } else {
                std::vector<SDL_Rect> empty_vector;
                return loadImage(empty_vector,not_found);
            }
        // Preprocessing
        } else {
            return_image.surface=tmp_surface;
            // if no description is given consider this as one big image
            if (image_desc.empty()) {
                SDL_Rect whole_image_rect;
                whole_image_rect.x=0;
                whole_image_rect.y=0;
                whole_image_rect.w=return_image.surface->w;
                whole_image_rect.h=return_image.surface->h;
                image_desc.push_back(whole_image_rect);
            }
            return_image.description=image_desc;
            // About ColorKeys: I'm not sure anymore what is necessary and what not, as some stuff is now
            // also done in SDL_rotozoom.cpp, at least it seems to work this way...
            // Image's colorkey have to be set before (if unset) as the SCALING changes the color values
            #ifdef ALPHA
            if (!(tmp_surface->flags & (SDL_SRCCOLORKEY|SDL_SRCALPHA))) {
            #else
            if (!(tmp_surface->flags & (SDL_SRCCOLORKEY))) {
            #endif
                SDL_SetColorKey(return_image.surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(return_image.surface->format,255,0,255));
            }
            // SCALING (we need to assign tmp_surface a value again to have a valid SDL_FreeSurface...)
            if (scale_factor!=1) tmp_surface=(scaleImage(return_image,scale_factor)).surface;
            // Image's colorkey
            if (return_image.surface->flags & SDL_SRCCOLORKEY) {
                SDL_SetColorKey(return_image.surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, return_image.surface->format->colorkey);
                return_image.surface=SDL_DisplayFormat(return_image.surface);
            #ifdef ALPHA
            // Alpha
            } else if (return_image.surface->flags & SDL_SRCALPHA) {
                SDL_SetAlpha(return_image.surface, SDL_SRCALPHA|SDL_RLEACCEL, return_image.surface->format->alpha);
                return_image.surface=SDL_DisplayFormatAlpha(return_image.surface);
            #endif
            // Our own colorkey (0xff00ff)
            } else {
                SDL_SetColorKey(return_image.surface, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(return_image.surface->format,255,0,255));
                return_image.surface=SDL_DisplayFormat(return_image.surface);
            }
            SDL_FreeSurface(tmp_surface);
        }
        // Nice return statement, isn't it ;-)))
        return (*(imgcache.insert(make_pair(make_pair(imagename,scale_factor),return_image))).first).second;
    // Return the existing Image in cache...
    } else return (*imgit).second;
}

Image& ImageCache::loadImage(string imagename, double scale_factor, string image_desc_file) {
    //negative values are taken as positive reciproc
    if (scale_factor<0) scale_factor=-1/scale_factor;
    if (image_desc_file.empty()) image_desc_file=imagename+".dsc";

    /* Parse image_desc_file */
    ifstream file;
    string tmpline;  
    string loadfile=config.datadir+image_desc_file;
    std::vector<SDL_Rect> description;
    SDL_Rect tmp_rect;

    file.open(loadfile.c_str());
    if (!file) {
        cout << "Failed to open the image description file: " << loadfile << " not found!" << endl;
    } else {
        cout << "Loading image description: " << loadfile << endl;

        string arg1,arg2,arg3,arg4,arg5,arg6;
        Uint16 description_type=DESC_NONE;

        while (getline(file,tmpline)) {
            arg1=arg2=arg3=arg4=arg5=arg6="";
            std::istringstream tmpstream(tmpline);
            tmpstream >> arg1 >> arg2 >> arg3 >> arg4 >> arg5 >> arg6;

            if        (arg1 == "ANIMATION") {
                description_type=DESC_NONE;
            } else if (arg1 == "DESCRIPTION") {
                if (arg2 == "LVLANIM") {
                    description_type=DESC_LVLANIM;
                } else if (arg2 == "FIX_RECT") {
                    if (arg3.empty() || arg4.empty() || arg5.empty() || arg6.empty()) {
                        description_type=DESC_NONE;
                    } else {
                        tmp_rect.x=atoi(arg3.c_str());
                        tmp_rect.y=atoi(arg4.c_str());
                        tmp_rect.w=atoi(arg5.c_str());
                        tmp_rect.h=atoi(arg6.c_str());
                        /* Load Image Case 2*/
                        return loadImage(tmp_rect,imagename,scale_factor);
                    }
                } else if (arg2 == "FIX_HOR_WS") {
                    if (arg3.empty() || arg4.empty()) {
                        description_type=DESC_NONE;
                    } else {
                        /* Load Image Case 3*/
                        return loadImage(atoi(arg3.c_str()),atoi(arg4.c_str()),imagename,scale_factor);
                    }
                } else if (arg2 == "FIX_HOR_NUM") {
                    if (arg3.empty()) {
                        description_type=DESC_NONE;
                    } else {
                        /* Load Image Case 4*/
                        return loadImage(atoi(arg3.c_str()),imagename,scale_factor);
                    }
                } else {
                    description_type=DESC_NONE;
                }
            }
        
            if (description_type==DESC_LVLANIM) {
                if (arg1.empty() || arg2.empty() || arg3.empty() || arg4.empty()) {
                } else {
                    tmp_rect.x=atoi(arg1.c_str());
                    tmp_rect.y=atoi(arg2.c_str());
                    tmp_rect.w=atoi(arg3.c_str());
                    tmp_rect.h=atoi(arg4.c_str());
                    description.push_back(tmp_rect);
                }
            }
        }

        file.close();
        file.clear();
    }

    /* Load Image Case 1 */
    return loadImage(description,imagename,scale_factor);
}

Image& ImageCache::loadImage(SDL_Rect base_rect, string imagename, double scale_factor) {
    //negative values are taken as positive reciproc
    if (scale_factor<0) scale_factor=-1/scale_factor;
    std::vector<SDL_Rect> description;
    if (imagename=="" || base_rect.w==0 || base_rect.h==0) return loadImage(description,imagename,scale_factor);
    Image& img=loadImage(description,imagename,scale_factor);
    if (scale_factor != 1) scaleRectangle(base_rect,scale_factor);
    SDL_Rect cur_rect=base_rect;

    Uint16 x=base_rect.x;
    Uint16 y=base_rect.y;
    while (y<=(img.surface->h-base_rect.h)) {
        cur_rect.y=y;
        x=base_rect.x;
        while (x<=(img.surface->w-base_rect.w)) {
            cur_rect.x=x;
            description.push_back(cur_rect);
            x+=base_rect.w;
        }
        y+=base_rect.h;
    }

    img.description=description;
    return img;
}

Image& ImageCache::loadImage(Uint16 num_frames, string imagename, double scale_factor) {
    //negative values are taken as positive reciproc
    if (scale_factor<0) scale_factor=-1/scale_factor;
    std::vector<SDL_Rect> description;
    Image& img=loadImage(description,imagename,scale_factor);
    SDL_Rect base_rectangle;
    base_rectangle.x=0;
    base_rectangle.y=0;
    base_rectangle.w=(Uint16)(img.surface->w*1.0/num_frames/scale_factor);
    base_rectangle.h=(Uint16)(img.surface->h*1.0/scale_factor);
    return loadImage(base_rectangle,imagename,scale_factor);
}

Image& ImageCache::loadImage(Uint16 width, Uint16 shift, string imagename, double scale_factor) {
    //negative values are taken as positive reciproc
    if (scale_factor<0) scale_factor=-1/scale_factor;
    std::vector<SDL_Rect> description;
    Image& img=loadImage(description,imagename,scale_factor);
    SDL_Rect base_rectangle;
    base_rectangle.x=shift;
    base_rectangle.y=0;
    base_rectangle.w=(Uint16)(width*1.0/scale_factor);
    base_rectangle.h=(Uint16)(img.surface->h*1.0/scale_factor);
    return loadImage(base_rectangle,imagename,scale_factor);
}
