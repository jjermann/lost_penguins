#ifndef DEF_IMGCACHE_H
#define DEF_IMGCACHE_H 1

#define DESC_NONE               0x00000000
#define DESC_LVLANIM            0x00000001

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
        //@{
        Image& loadImage(std::vector<SDL_Rect> image_desc, string imagename, double scale_factor=1);
        Image& loadImage(string imagename, double scale_factor=1, string image_desc_file="");
        Image& loadImage(Uint16 num_frames, string imagename, double scale_factor=1);
        Image& loadImage(Uint16 width, Uint16 shift, string imagename, double scale_factor=1);
        Image& loadImage(SDL_Rect base_rect, string imagename, double scale_factor=1);
        //@}
    private:
        std::map<pair<string,double>,Image> imgcache;
        string not_found;
    private:
        SDL_Rect& scaleRectangle(SDL_Rect& base_rect, double scale_factor);
        std::vector<SDL_Rect>& scaleDescription(std::vector<SDL_Rect>& description, double scale_factor);
        Image& scaleImage(Image& original_image, double scale_factor);
};

#endif
