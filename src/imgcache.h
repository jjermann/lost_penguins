#ifndef _IMGCACHE_H
#define _IMGCACHE_H 1

class ImageCache {
    public:
        ImageCache();
        ~ImageCache();
        SDL_Surface* loadImage(string imagename="");
    private:
        std::map<string,SDL_Surface*> imgcache;
        string not_found;
};

#endif
