#ifndef _IMGCACHE_H_
    #define _IMGCACHE_H_
#endif

class ImageCache {
    public:
        ImageCache();
        ~ImageCache();
        SDL_Surface* loadImage(string imagename);
    private:
        std::map<string,SDL_Surface*> imgcache;    
};
