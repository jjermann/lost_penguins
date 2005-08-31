#include "common.h"
#include "scenario.h"
//HACK
#include "gfxeng.h"
#include "sfxeng.h"
#include "sndcache.h"
#include "objectpools.h"
//HACK!!!!!!
#include "monsters_common.h"
#include "objects/bomb.h"
#include "objects/door.h"
#include "objects/exit.h"
#include "objects/geyser.h"
#include "objects/heart.h"
#include "objects/key.h"
#include "objects/teleport.h"
#include "objects/trigger.h"
#include "objects/wall.h"
#include "objects/water.h"
#include "objects/wind.h"
#include "objects/spike.h"
//Characters
#include "objects/triggered_bomb.h"
#include "objects/plant.h"
#include "objects/baleog.h"
#include "objects/erik.h"
#include "objects/olaf.h"
#include "objects/scorch.h"
#include "objects/fang.h"
#include "objects/zombie.h"


ObjectsPool::ObjectsPool():
  currentplayer(playerspool.begin()),
  au_switch(sndcache->loadWAV("newchar.wav")) { }
ObjectsPool::~ObjectsPool() {
    object_iterator i=objectspool.begin();
    while (i!=objectspool.end()) i=removeObject(i);
}

Object* ObjectsPool::addObjectbyName(const string& obj, const string& image, Sint16 x, Sint16 y, const string& arg1, const string& arg2, const string& arg3) {
    x-=gfxeng->getShift().x;
    y-=gfxeng->getShift().y;
    //Set names...
    string name=obj;
    if (arg1!="0") {
        //one additional parameter
        if (name=="Trigger" || name=="Door" || name=="Bomb" || name=="TriggeredBomb" || name=="Plant" || name=="Geyser" || name=="Wind" || name=="Spike") {
            if (arg2!="0") name=arg2;
        //two additional parameter
        } else if (name=="Teleporter") {
            if (!arg3.empty()) name=arg3;
        //no additional parameters
        } else {
            name=arg1;
        }
    }
    //normal objects
    if (obj=="Wall")         return (addObject(new Wall(image,x,y,name)));
    else if (obj=="Exit")    return (addObject(new Exit(image,x,y,name)));
    else if (obj=="Water")   return (addObject(new Water(image,x,y,name)));
    else if (obj=="Teleporter") return (addObject(new Teleporter(image,x,y,atoi(arg1.c_str()),atoi(arg2.c_str()),name)));
    else if (obj=="Wind")    return (addObject(new Wind(image,x,y,atoi(arg1.c_str()),name)));
    else if (obj=="Geyser")  return (addObject(new Geyser(image,x,y,atoi(arg1.c_str()),name)));
    else if (obj=="Trigger") return (addObject(new Trigger(image,x,y,arg1,name,arg3)));
    else if (obj=="Door")    return (addObject(new Door(image,x,y,arg1,name)));
    else if (obj=="Spike")   return (addObject(new Spike(image,x,y,atoi(arg1.c_str()),name)));
    //Items
    else if (obj=="Heart")   return (addObject(new Heart(image,x,y,name)));
    else if (obj=="Key")     return (addObject(new Key(image,x,y,name)));
    else if (obj=="Bomb")    return (addObject(new Bomb(image,x,y,atoi(arg1.c_str()),name)));

    //normal characters
    else if (obj=="TriggeredBomb") return (addCharacter(new TriggeredBomb(image,x,y,atoi(arg1.c_str()),name)));

    //Players
    else if (obj=="Erik")    return (addPlayer(new Erik(image,x,y,name)));
    else if (obj=="Olaf")    return (addPlayer(new Olaf(image,x,y,name)));
    else if (obj=="Baleog")  return (addPlayer(new Baleog(image,x,y,name)));
    else if (obj=="Fang")    return (addPlayer(new Fang(image,x,y,name)));  
    else if (obj=="Scorch")  return (addPlayer(new Scorch(image,x,y,name)));

    //Monsters
    else if (obj=="Plant")   return (addMonster(new Plant(image,x,y,atoi(arg1.c_str()),name)));
    else if (obj=="Zombie")  return (addMonster(new Zombie(image,x,y,name)));

    //not recognized
    else {
        cout << "Object " << obj << " unknown, skipping...\n";
        return NULL;
    }
}

Object* ObjectsPool::getObject(const string& oname) {
    object_iterator i=objectspool.begin();
    while (i != objectspool.end()) {
        if ((*i)->getName()==oname) return (*i);
        ++i;
    }
    return NULL;
}

Object* ObjectsPool::addObject(Object* object) {
    if ( (object!=NULL) && (scenario->area==NULL || (scenario->checkPlace(*(object->getPos()),*scenario->area).enter==NOTHING)) ) {
        objectspool.insert(object);
        return object;
    } else {
        cout << "Couldn't place object!\n";
        return NULL;
    }
}

string ObjectsPool::getNextObjectName(const string& basename) {
    int objnum=1;
    while(getObject(basename+itos(objnum))) {
        objnum++;
    }
    return basename+itos(objnum);
}

//yeah, a pyramide... ;) serious, this is _NOT_ good
//hmm, but it can be used as a bad example on how "_not_ to do things"... ;-)
///\todo Fix this mess
object_iterator ObjectsPool::removeObject(object_iterator it) {
    if (it!=objectspool.end()) {
        Object* obj=*it;
        objectspool.erase(obj);
        if (Character* ptrc = dynamic_cast<Character*>(obj)) {
            characterspool.erase(ptrc);
            if(Player* ptrv = dynamic_cast<Player*>(obj)) {
                if (playerspool.erase(ptrv)) {
                    currentplayer=playerspool.begin();
                    if (currentplayer!=playerspool.end()) {
                        if (scenario->player) scenario->player->clearStates(false);
                        scenario->player=*currentplayer;
                    } else scenario->player=NULL;
                }
            } else if (Monster* ptrm = dynamic_cast<Monster*>(obj)) monsterspool.erase(ptrm);
        }
        (obj)->destroy();
        return (objectspool.begin());
    } else return objectspool.end();
}

object_iterator ObjectsPool::removeObject(Object* object) {
    object_iterator tmpit=objectspool.find(object);
    return removeObject(tmpit);
}

Object* ObjectsPool::moveObject(Object* object) {
    objectspool.erase(object);
    if (Character* ptrc = dynamic_cast<Character*>(object)) {
        characterspool.erase(ptrc);
        if(Player* ptrv = dynamic_cast<Player*>(object)) {
            if (playerspool.erase(ptrv)) {
                currentplayer=playerspool.begin();
                if (currentplayer!=playerspool.end()) {
                    if (scenario->player) scenario->player->clearStates(false);
                    scenario->player=*currentplayer;
                } else scenario->player=NULL;
            }
        } else if (Monster* ptrm = dynamic_cast<Monster*>(object)) monsterspool.erase(ptrm);
    }
    return object;
}


Character* ObjectsPool::addCharacter(Character* newcharacter) {
    if ( (newcharacter!=NULL) && (scenario->checkPlace(*(newcharacter->getPos()),*scenario->area).enter==NOTHING) ) {
        characterspool.insert(newcharacter);
        scenario->pool->addObject(newcharacter);
        return newcharacter;
    } else {
        cout << "Couldn't place character!\n";
        return NULL;
    }
}

Player* ObjectsPool::addPlayer(Player* newplayer) {
    if ( (newplayer!=NULL) && (scenario->checkPlace(*(newplayer->getPos()),*scenario->area).enter==NOTHING) ) {
        playerspool.insert(newplayer);
        currentplayer=playerspool.begin();
        scenario->pool->addCharacter(newplayer);
        return newplayer;
    } else {
        cout << "Couldn't place player!\n";
        return NULL;
    }
}

Player* ObjectsPool::switchPlayer() {
    if (currentplayer != playerspool.end()) {
        sfxeng->playWAV(au_switch);
        ++currentplayer;
        if (currentplayer == playerspool.end()) currentplayer=playerspool.begin();
        if (currentplayer != playerspool.end()) {
            if (scenario->player) scenario->player->clearStates(false);
            scenario->player=*currentplayer;
        }
        else scenario->player=NULL;
        return scenario->player;
    } else return (scenario->player=NULL);
}


Monster* ObjectsPool::addMonster(Monster* newmonster) {
    if ( (newmonster!=NULL) && (scenario->checkPlace(*(newmonster->getPos()),*scenario->area).enter==NOTHING) ) {
        monsterspool.insert(newmonster);
        scenario->pool->addCharacter(newmonster);
        return newmonster;
    } else {
        cout << "Couldn't place monster!\n";
        return NULL;
    }
}

bool ObjectsPool::empty() {
    if (scenario->pool->objectspool.empty()) return true;
    else return false;
}
