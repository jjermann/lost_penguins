#ifndef DEF_SCENARIO_H
#define DEF_SCENARIO_H 1

/** \brief Scenario organization

    Loads the corresponding map data (objects) and performs location checks.
    \remarks MAP FORMAT:
    \remarks HeaderOption "parameters"
    \remarks Background "parameters"
    \remarks \#ENDHEADER
    \remarks "Object class name" "x position" "y position" "parameters"
    \remarks The parameters depend on the object: Each object checks it's
      corresponding parameters.
    \remarks The parameter format is like this:
    \remarks option1=value1,option2=value2,...
    \remarks A value may be a parameterlist of suboptions:
    \remarks option1=subopt1=subval1:subopt2=subval2:...,option2=value2
    \remarks Defaults:
    \remarks x and y default to 0
    \remarks Example:
    \remarks Background image=background.bmp
    \remarks \#ENDHEADER
    \remarks Teleporter 0 0 image=teleporter.bmp,exitx=20,exity=100,name=teleporter1
    \todo Move the Caches into the current map as well?
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
        std::set<Object *> getObjectsIn(Uint16 mask, const SDL_Rect& rect, bool touch=false, Uint16 radius=0, Uint16 dir=DIR_ALL) const;
        /// Returns the first found object at the given position
        Object* getObjectAt(Sint16 x, Sint16 y, Uint16 mask=ALL) const;
        /// Returns the directions from the source rectangle to the destination rectangle
        /// \remark This may include an enlargment (eg. Olaf)
        /// \param src Source rectangle
        /// \param dest Destination rectangle
        Uint16 getDirection(const SDL_Rect& src, const SDL_Rect& dest) const;
        /// Load a new scenario
        int loadScenario(string scenarioname);

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
        /// Create a new empty map with the specified background
        void newMap(const ParameterMap& bg_parameters);
        /// Used to load the map file into a buffer
        int loadMapBuf(string mapname);
        /// Reload the map file using the buffered map file
        int reloadMap();
        ///\brief Loads and initializes the map data
        ///
        /// Parses the map file and tries to add the objects by using addObjectByName()
        /// \param mapname Map file name without the data directory in it
        /// \return 0 if a Background was found, -1 if not, -2 if the loading failed
        int loadMap(string mapname);
        int loadMap(Uint8 level=0);
        void loadNextMap();
        int startScenario();
        void winScenario();
        void resetScenario();
        /// True if the mission failed
        bool failed;
        /// True if the mission is finnished (suceeded or failed)
        bool finnished;
        /// Name of the current map file
        string mapname;
        /// Name of the scenario
        string scenarioname;
        /// Name of the map background image
        string bgimage;
        std::vector<string> mapbuf;
        /// The number of created objects
        Uint32 max_obj_num;
        std::vector<string> maps;
        std::vector<string> playlist;
        Uint8 currentmap;
    private:
        inline void reinitMap();
};

#endif
