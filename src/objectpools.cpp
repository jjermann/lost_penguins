#include "common.h"
#include "map.h"
#include "sfxeng.h"
#include "sndcache.h"
#ifndef _OBJECTPOOLS_H_
    #include "objectpools.h"
#endif
//HACK!!!!!!
#include "objects/baleog.h"
#include "objects/bomb.h"
#include "objects/door.h"
#include "objects/eric.h"
#include "objects/exit.h"
#include "objects/fang.h"
#include "objects/geyser.h"
#include "objects/heart.h"
#include "objects/key.h"
#include "objects/olaf.h"
#include "objects/scorch.h"
#include "objects/teleport.h"
#include "objects/trigger.h"
#include "objects/triggered_bomb.h"
#include "objects/wall.h"
#include "objects/water.h"
#include "objects/wind.h"
#include "objects/zombie.h"


/*==================
    Objectspool
==================*/

ObjectsPool::ObjectsPool():
  currentviking(vikingspool.begin()) {
    au_switch=sndcache->loadWAV("newchar.wav");
}
ObjectsPool::~ObjectsPool() {
    object_iterator i=objectspool.begin();
    while (i!=objectspool.end()) i=removeObject(i);
}

//PRE:  name of object (obj), imagename of object (image), placing position of object (x,y)
//ACT:  adds the object to the corresponding pool if it was recognized
//POST: return a pointer to the created object, return NULL if not recognized
Object* ObjectsPool::addObjectbyName(const string& obj, const string& image, Uint16 x, Uint16 y, const string& arg1, const string& arg2, const string& arg3) {
    //Set names...
    string name=obj;
    if (arg1!="0") {
        //one additional parameter
        if (name=="Trigger" || name=="Door" || name=="Bomb" || name=="TriggeredBomb" || name=="Geyser" || name=="Wind") {
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
    //Items
    else if (obj=="Heart")   return (addObject(new Heart(image,x,y,name)));
    else if (obj=="Key")     return (addObject(new Key(image,x,y,name)));
    else if (obj=="Bomb")    return (addObject(new Bomb(image,x,y,atoi(arg1.c_str()),name)));

    //normal characters
    else if (obj=="TriggeredBomb") return (addCharacter(new TriggeredBomb(image,x,y,atoi(arg1.c_str()),name)));
    //Vikings
    else if (obj=="Eric")    return (addViking(new Eric(image,x,y,name)));
    else if (obj=="Olaf")    return (addViking(new Olaf(image,x,y,name)));
    else if (obj=="Baleog")  return (addViking(new Baleog(image,x,y,name)));
    else if (obj=="Fang")    return (addViking(new Fang(image,x,y,name)));  
    else if (obj=="Scorch")  return (addViking(new Scorch(image,x,y,name)));

    //Monsters
    else if (obj=="Zombie")  return (addMonster(new Zombie(image,x,y,name)));

    //not recognized
    else {
        cout << "Object " << obj << " unknown, skipping...\n";
        return NULL;
    }
}

//PRE:  object name, MUST be unique
//POST: pointer to object or NULL
Object* ObjectsPool::getObject(const string& oname) {
    object_iterator i=objectspool.begin();
    while (i != objectspool.end()) {
        if ((*i)->getName()==oname) return (*i);
        ++i;
    }
    return NULL;
}

//FIX: find a way to detect failed image loading (rather in Object constructor?)
Object* ObjectsPool::addObject(Object* object) {
    if ( (object!=NULL) && (maparea==NULL || (curmap->checkPlace(*(object->getPos()),*maparea).enter==NOTHING)) ) {
        objectspool.insert(object);
        return object;
    } else {
        cout << "Couldn't place object!\n";
        return NULL;
    }
}

//yeah, a pyramide... ;) serious, this is _NOT_ good
//hmm, but it can be used as a bad example on how "_not_ to do things"... ;-)
object_iterator ObjectsPool::removeObject(object_iterator it) {
    if (it!=objectspool.end()) {
        objectspool.erase(*it);
        if (Character* ptrc = dynamic_cast<Character*>(*it)) {
            characterspool.erase(ptrc);
            if(Viking* ptrv = dynamic_cast<Viking*>(*it)) {
                if (vikingspool.erase(ptrv)) {
                    currentviking=vikingspool.begin();
                    if (currentviking!=vikingspool.end()) {
                        if (viking) viking->clearStates(false);
                        viking=*currentviking;
                    } else viking=NULL;
                }
            } else if (Monster* ptrm = dynamic_cast<Monster*>(*it)) monsterspool.erase(ptrm);
        }
        delete (*it);
        return (++it);
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
        if(Viking* ptrv = dynamic_cast<Viking*>(object)) {
            if (vikingspool.erase(ptrv)) {
                currentviking=vikingspool.begin();
                if (currentviking!=vikingspool.end()) {
                    if (viking) viking->clearStates(false);
                    viking=*currentviking;
                } else viking=NULL;
            }
        } else if (Monster* ptrm = dynamic_cast<Monster*>(object)) monsterspool.erase(ptrm);
    }
    return object;
}


//only used internally by the objects -> Fix!!
Character* ObjectsPool::addCharacter(Character* newcharacter) {
    if ( (newcharacter!=NULL) && (curmap->checkPlace(*(newcharacter->getPos()),*maparea).enter==NOTHING) ) {
        characterspool.insert(newcharacter);
        pool->addObject(newcharacter);
        return newcharacter;
    } else {
        cout << "Couldn't place character!\n";
        return NULL;
    }
}

//only used internally by the objects -> Fix!!
Viking* ObjectsPool::addViking(Viking* newviking) {
    if ( (newviking!=NULL) && (curmap->checkPlace(*(newviking->getPos()),*maparea).enter==NOTHING) ) {
        vikingspool.insert(newviking);
        currentviking=vikingspool.begin();
        pool->addCharacter(newviking);
        return newviking;
    } else {
        cout << "Couldn't place viking!\n";
        return NULL;
    }
}

//POST: rotates vikings returns a non NULL pointer if possible
Viking* ObjectsPool::switchViking() {
    if (currentviking != vikingspool.end()) {
        sfxeng->playWAV(au_switch);
        ++currentviking;
        if (currentviking == vikingspool.end()) currentviking=vikingspool.begin();
        if (currentviking != vikingspool.end()) {
            if (viking) viking->clearStates(false);
            viking=*currentviking;
        }
        else viking=NULL;
        return viking;
    } else return (viking=NULL);
}


//only used internally by the objects -> Fix!!
Monster* ObjectsPool::addMonster(Monster* newmonster) {
    if ( (newmonster!=NULL) && (curmap->checkPlace(*(newmonster->getPos()),*maparea).enter==NOTHING) ) {
        monsterspool.insert(newmonster);
        pool->addCharacter(newmonster);
        return newmonster;
    } else {
        cout << "Couldn't place monster!\n";
        return NULL;
    }
}
