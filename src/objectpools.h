#ifndef _OBJECTPOOLS_H
#define _OBJECTPOOLS_H

class ObjectsPool {
    public:
        ObjectsPool();
        ~ObjectsPool();
        //add objects
        Object* addObjectbyName(const string& obj, const string& image, Uint16 x=0, Uint16 y=0, const string& arg1="0", const string& arg2="0", const string& arg3="0");
        Object*            addObject(Object* object);
        Character*         addCharacter(Character* newcharacter);
        Player*            addPlayer(Player* newplayer);
        Monster*           addMonster(Monster* newmonster);
        //get objects
        Object*            getObject(const string& oname);
        //(re)move objects
        object_iterator    removeObject(object_iterator it);
        object_iterator    removeObject(Object* object);
        Object*            moveObject(Object* object);

        Player* switchPlayer();

        //pools
        std::set<Object *>    objectspool;
        std::set<Character *> characterspool;
        std::set<Player *>    playerspool;
        std::set<Monster *>   monsterspool;
    private:
        //player number of the currently selected player
        player_iterator currentplayer;
        Mix_Chunk* au_switch;
};

#endif
