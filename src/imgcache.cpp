#include "common.h"
#include "imgcache.h"


ImageCache::ImageCache() {
    not_found="no_file.bmp";
}
ImageCache::~ImageCache() {
    map<string, SDL_Surface*>::iterator imgit=imgcache.begin();
    while (imgit != imgcache.end()) {
        SDL_FreeSurface((*imgit).second);
        imgcache.erase(imgit++);
    }
}

SDL_Surface* ImageCache::loadImage(string imagename) {
    //default is not_found
    if (imagename=="") imagename=not_found;
    map<string, SDL_Surface*>::iterator imgit=imgcache.find(imagename);
    
    //load new image
    if (imgit==imgcache.end()) {
        string loadfile=config.datadir+imagename;
        SDL_Surface* tmpimg=NULL;
        SDL_Surface* returnimg=NULL;
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
        } else {
            if (tmpimg->flags & SDL_SRCCOLORKEY) {
                SDL_SetColorKey(tmpimg, SDL_SRCCOLORKEY|SDL_RLEACCEL, tmpimg->format->colorkey);
                returnimg=SDL_DisplayFormat(tmpimg);
            #ifdef ALPHA
            } else if (tmpimg->flags & SDL_SRCALPHA) {
                SDL_SetAlpha(tmpimg, SDL_SRCALPHA|SDL_RLEACCEL, tmpimg->format->alpha);
                returnimg=SDL_DisplayFormatAlpha(tmpimg);
            #endif
            } else {
                SDL_SetColorKey(tmpimg, SDL_SRCCOLORKEY|SDL_RLEACCEL, SDL_MapRGB(tmpimg->format,255,0,255));
                returnimg=SDL_DisplayFormat(tmpimg);
            }
            SDL_FreeSurface(tmpimg);
        }
        imgcache.insert(make_pair(imagename,returnimg));
        return returnimg;
    //already there
    } else return (*imgit).second;
}
