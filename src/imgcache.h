#ifndef _IMGCACHE_H
#define _IMGCACHE_H 1

/** \brief Caches images.

    Caches images supported by SDL_image according to their name
    (using a map container) and deletes them at the end.
*/
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
