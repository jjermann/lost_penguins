#ifndef DEF_OBJECTPOOLS_H
#define DEF_OBJECTPOOLS_H 1

struct Compare {
    bool operator()(const Object*, const Object*);
};

/** \brief Container for objects.

    Contains a pool for each important derived object class,
    manages adding/deleting objects and switching players.
    Each derived type is contained in all it's super type pools.
    \todo Move the Object creation code to the objects and remove all
      dependencies to the objects/ headers.
    \todo Cleanup the whole mess.
*/
class ObjectsPool {
    public:
        ObjectsPool();
        ~ObjectsPool();
        ///\brief sets the static default_parameters variable of all objects
        void setDefaultObjParam();
        ///\brief Returns the default_parameters of the corresponding obj or an empty map
        const ParameterMap& getDefaultObjParambyName(const string& obj);
        ///\brief Returns true if no objects were loaded so far...
        bool empty();
        ///\todo Move this code to the objects
        ///\brief Add objects by their names
        ///
        /// Add an Object by it's name to the corresponding pool if it was
        /// recognized, initialized with the parsed options.
        /// \param obj Name of the Object class
        /// \param x x coordinate
        /// \param y y coordinate
        /// \param parameters A map<string,string> of all parameters
        /// \param outside True if the object shouldn't be added to the pools
        /// \return Pointer to the new entry in the objectspool or NULL if
        ///   the object was not recognized
        Object* addObjectbyName(const string& obj, Sint16 x=0, Sint16 y=0, ParameterMap& parameters=ParameterMap(), bool outside=false);
        /// Add an Object to all corresponding pools
        /// \return Pointer to the new entry in the objectspool or NULL if it failed
        Object* addObject(Object* object, bool outside=false);
        /// Gets an object by it's name
        /// \pre The name must be unique (otherwise it's basically random)
        /// \return Pointer to the Object or NULL if it wasn't found
        Object* getObject(const string& oname);
        /// Helper function to return the next available object name corresponding to the given base name
        string getNextObjectName(const string& basename);
        //@{
        /// Remove an Object (using an object_iterator) from all pools it belongs to
        /// \return object_iterator to the next entry in the pool or the end()
        object_iterator removeObject(object_iterator it);
        /// Remove an Object (using a pointer to it)
        /// \return object_iterator to the next entry in the pool or the end()
        object_iterator removeObject(Object* object);
        /// Detaches an Object from the objectspool
        /// \return Pointer to the detached Object
        Object* moveObject(Object* object);
        //@}
        /// Selects a new current player (circular from left to right) if possible
        Player* switchPlayer();

        //@{
        ///\brief Object pool
        ///
        /// Contains all objects managed by the ObjectsPool
        std::set<Object *,Compare>    objectspool;
        ///\brief Character pool
        ///
        /// Contains all characters managed by the ObjectsPool
        std::set<Character *,Compare> characterspool;
        ///\brief Player pool
        ///
        /// Contains all players managed by the ObjectsPool
        std::set<Player *,Compare>    playerspool;
        ///\brief Monster pool
        ///
        /// Contains all monsters managed by the ObjectsPool
        std::set<Monster *,Compare>   monsterspool;
        //@}
    private:
        //player number of the currently selected player
        player_iterator currentplayer;
        Mix_Chunk* au_switch;
        const ParameterMap empty_parameter;
};

#endif
