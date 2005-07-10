#ifndef DEF_OBJECTPOOLS_H
#define DEF_OBJECTPOOLS_H 1

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
        ///\brief Returns true if no objects were loaded so far...
        bool empty();
        ///\todo Move this code to the objects
        ///\brief Add objects by their names
        ///
        /// Add an Object by it's name to the corresponding pool if it was
        /// recognized, initialized with the parsed options.
        /// \param obj Name of the Object class
        /// \param image Name of the base image to be load
        /// \param x x coordinate
        /// \param y y coordinate
        /// \param arg1 First argument as string
        /// \param arg2 Second argument as string
        /// \param arg3 Third argument as string
        /// \return Pointer to the new entry in the objectspool or NULL if
        ///   the object was not recognized
        Object* addObjectbyName(const string& obj, const string& image, Sint16 x=0, Sint16 y=0, const string& arg1="0", const string& arg2="0", const string& arg3="0");
        //@{
        /// Add an Object to the objectspool
        /// \return Pointer to the new entry in the objectspool or NULL if it failed
        Object*            addObject(Object* object);
        /// Add a Character to the characterspool (and objectspool)
        /// \return Pointer to the new entry in the characterspool or NULL if it failed
        Character*         addCharacter(Character* newcharacter);
        /// Add a Player to the playerspool (and objects/characterspool)
        /// \return Pointer to the new entry in the playerspool or NULL if it failed
        Player*            addPlayer(Player* newplayer);
        /// Add a Monster to the monsterspool
        /// \return Pointer to the new entry in the monsterspool or NULL if it failed
        Monster*           addMonster(Monster* newmonster);
        //@}
        /// Gets an object by it's name
        /// \pre The name must be unique (otherwise it's basically random)
        /// \return Pointer to the Object or NULL if it wasn't found
        Object*            getObject(const string& oname);
        //@{
        /// Remove an Object (using an object_iterator) from all pools it belongs to
        /// \return object_iterator to the next entry in the pool or the end()
        object_iterator    removeObject(object_iterator it);
        /// Remove an Object (using a pointer to it)
        /// \return object_iterator to the next entry in the pool or the end()
        object_iterator    removeObject(Object* object);
        /// Detaches an Object from the objectspool
        /// \return Pointer to the detached Object
        Object*            moveObject(Object* object);
        //@}
        /// Selects a new current player (circular from left to right) if possible
        Player* switchPlayer();

        //@{
        ///\brief Object pool
        ///
        /// Contains all objects managed by the ObjectsPool
        std::set<Object *>    objectspool;
        ///\brief Character pool
        ///
        /// Contains all characters managed by the ObjectsPool
        std::set<Character *> characterspool;
        ///\brief Player pool
        ///
        /// Contains all players managed by the ObjectsPool
        std::set<Player *>    playerspool;
        ///\brief Monster pool
        ///
        /// Contains all monsters managed by the ObjectsPool
        std::set<Monster *>   monsterspool;
        //@}
    private:
        //player number of the currently selected player
        player_iterator currentplayer;
        Mix_Chunk* au_switch;
};

#endif
