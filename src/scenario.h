#ifndef _MAP_H
#define _MAP_H 1

/** \brief Scenario organization

    Loads the corresponding map data (objects) and performs location checks.
    \remarks MAP FORMAT:
    \remarks "Object class name" "Base image name" "x position" "y position"
      "Arg1" "Arg2" "Arg3"
    \remarks Each Object handels the arguments for itself, usually "Arg1"
      specifies the object name (if the object has no further parameters)
    \remarks Defaults:
    \remarks x and y default to 0
    \remarks Example:
    \remarks Background background.bmp
    \remarks Teleporter teleporter.bmp 0 0 20 100 teleporter1
    \todo Improve the map format (eg. support headers)
    \todo Move the Caches into the current map as well?
    \todo Create a map class inside Scenario?
*/
class Scenario {
    public:
        /// Creates a new scenario. To load a specific map, use loadMap(mapname).
        Scenario();
        ~Scenario();
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
        /// \remark This may include an enlargment (eg. Olaf)
        /// \param src Source rectangle
        /// \param dest Destination rectangle
        Uint16 getDirection(const SDL_Rect& src, const SDL_Rect& dest) const;

        /// Background
        Background* background;
        /// Size of the background (map area)
        SDL_Rect* area;
        /// Current player
        Player* player;
        /// Image Cache
        ImageCache* imgcache;
        /// Sound Cache
        SoundCache* sndcache;
        /// Object pool
        ObjectsPool* pool;
        /// Physic Handler
        PhysicHandler* physic;
        ///\brief Loads and initializes the map data
        ///
        /// Parses the map file and tries to add the objects by using addObjectByName()
        /// \param mapname Map file name without the data directory in it
        /// \return 0 if a Background was found, -1 if not, -2 if the loading failed
        int loadMap(string mapname);
        /// True if the mission failed
        bool failed;
        /// Name of the map file
        string name;
    private:
        inline void reinitMap();
};

#endif
