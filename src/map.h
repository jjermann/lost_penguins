#ifndef _MAP_H_
    #define _MAP_H_
#endif

class Map {
    public:
        Map(string mapname);
        ~Map();
        Hit checkPlace(const SDL_Rect& src, const SDL_Rect& dest) const;
        std::set<Character *> getCharactersIn(const SDL_Rect& rect, bool touch=false, Uint16 radius=0) const;
        std::set<Viking *> getVikingsIn(const SDL_Rect& rect, bool touch=false, Uint16 radius=0) const;
        Uint16 getDirection(const SDL_Rect& src, const SDL_Rect& dest) const;
    private:
        //map loading
        inline int loadMapData(const string& mapname);
};