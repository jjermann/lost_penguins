#ifndef _MAP_H
#define _MAP_H 1

/** \brief Map organization

    Loads the corresponding map data (objects) and performs location checks.
*/
class Map {
    public:
        Map(string mapname);
        ~Map();
        Hit checkPlace(const SDL_Rect& src, const SDL_Rect& dest) const;
        std::set<Character *> getCharactersIn(Uint16 mask, const SDL_Rect& rect, bool touch=false, Uint16 radius=0, Uint16 dir=DIR_ALL) const;
        Uint16 getDirection(const SDL_Rect& src, const SDL_Rect& dest) const;
    private:
        //map loading
        inline int loadMapData(const string& mapname);
};

#endif
