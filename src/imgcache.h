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
        /// Loads an image if it isn't already contained in the pool
        /// according to it's image name. If the image is not found
        /// it tries to load a fallback image (not_found).
        SDL_Surface* loadImage(string imagename="");
    private:
        std::map<string,SDL_Surface*> imgcache;
        string not_found;
};

#endif