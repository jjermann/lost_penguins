#ifndef _MAP_H
#define _MAP_H 1

/** \brief Map organization

    Loads the corresponding map data (objects) and performs location checks.
*/
class Map {
    public:
        Map(string mapname);
        ~Map();
        /// Checks where the source rectangle overlaps the destination rectangle.
        /// \param src Source rectangle
        /// \param dest Destination rectangle
        /// \return Hit Directions of overlapping. DIR_ALL if the destination rectangle is inside
        ///   the source rectangle, NOTHING if the source rectangle is inside the destionation
        ///   rectangle.
        Hit checkPlace(const SDL_Rect& src, const SDL_Rect& dest) const;
        /// \param mask Mask for the object types that this function should match
        ///   (default: all Characters).
        /// \param rect Base region of the target area that should be checked.
        /// \param touch If true an touch (overlapping of area) is enough for a match, otherwise
        ///   the center of the specified rectangle has to be inside the target.
        /// \param radius This enlarges the base area by this amount into all specified directions
        ///   (default: 0)
        /// \param dir Search directions (default: all)
        /// \return A set of characters that are inside the enlarged source rectangle
        ///   and that match the specified mask.
        std::set<Character *> getCharactersIn(Uint16 mask, const SDL_Rect& rect, bool touch=false, Uint16 radius=0, Uint16 dir=DIR_ALL) const;
        /// Returns the directions from the source rectangle to the destination rectangle
        /// \param src Source rectangle
        /// \param dest Destination rectangle
        Uint16 getDirection(const SDL_Rect& src, const SDL_Rect& dest) const;
    private:
        inline int loadMapData(const string& mapname);
};

#endif
