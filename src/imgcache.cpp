#include "common.h"
#include "SDL_rotozoom.h"
#include "imgcache.h"


ImageCache::ImageCache() {
    not_found="no_file.bmp";
}
ImageCache::~ImageCache() {
    map<pair<string,double>, SDL_Surface*>::iterator imgit=imgcache.begin();
    while (imgit != imgcache.end()) {
        SDL_FreeSurface((*imgit).second);
        imgcache.erase(imgit++);
    }
}

//TODO: maybe replace double scale_factor by string scale_format or sthg like that
SDL_Surface* ImageCache::loadImage(string imagename, double scale_factor) {
    //default is not_found
    if (imagename=="") {
        imagename=not_found;
        scale_factor=1;
    }
        
    map<pair<string,double>, SDL_Surface*>::iterator imgit=imgcache.find(make_pair(imagename,scale_factor));
    
    // Image is not yet cached
    if (imgit==imgcache.end()) {
        string loadfile=config.datadir+imagename;
        SDL_Surface* tmpimg=NULL;
        SDL_Surface* returnimg=NULL;
        // Loading
        #ifdef SDL_IMAGE
        if ((tmpimg=IMG_Load(loadfile.c_str())) == NULL) {
        #else
        if ((tmpimg=SDL_LoadBMP(loadfile.c_str())) == NULL) {
        #endif
            cout << "Couldn't load the image: " << imagename << endl;
            //Even the fallback image was not found
            if (not_found==imagename) {
                quitGame(3);
            //Try to load the fallback image...
            } else {
                return loadImage(not_found);
            }
        // Preprocessing
        } else {
            // About ColorKeys: I'm not sure anymore what is necessary and what not, as some stuff is now
            // also done in SDL_rotozoom.cpp, at least it seems to work this way...
            // Image's colorkey have to be set before (if unset) as the SCALING changes the color values
            #ifdef ALPHA
            if (!(tmpimg->flags & (SDL_SRCCOLORKEY|SDL_SRCALPHA))) {
            #else
            if (!(tmpimg->flags & (SDL_SRCCOLORKEY))) {
            #endif
                SDL_SetColorKey(tmpimg, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmpimg->format,255,0,255));
            }
            // SCALING
            if (scale_factor!=1) tmpimg=scaleImage(tmpimg,scale_factor);
            // Image's colorkey
            if (tmpimg->flags & SDL_SRCCOLORKEY) {
                SDL_SetColorKey(tmpimg, SDL_SRCCOLORKEY|SDL_RLEACCEL, tmpimg->format->colorkey);
                returnimg=SDL_DisplayFormat(tmpimg);
            #ifdef ALPHA
            // Alpha
            } else if (tmpimg->flags & SDL_SRCALPHA) {
                SDL_SetAlpha(tmpimg, SDL_SRCALPHA|SDL_RLEACCEL, tmpimg->format->alpha);
                returnimg=SDL_DisplayFormatAlpha(tmpimg);
            #endif
            // Our own colorkey (0xff00ff)
            } else {
                SDL_SetColorKey(tmpimg, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmpimg->format,255,0,255));
                returnimg=SDL_DisplayFormat(tmpimg);
            }
            SDL_FreeSurface(tmpimg);
        }
        imgcache.insert(make_pair(make_pair(imagename,scale_factor),returnimg));
        return returnimg;
    // Return the existing Image in cache...
    } else return (*imgit).second;
}

inline SDL_Surface* ImageCache::scaleImage(SDL_Surface* original_image, double scale_factor) {
    SDL_Surface* scaled_image=zoomSurface(original_image, scale_factor, scale_factor, 1);
    SDL_FreeSurface(original_image);
    return scaled_image;
}
