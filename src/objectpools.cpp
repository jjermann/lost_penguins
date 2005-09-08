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


ParameterMap Object::default_parameters;
ParameterMap Background::default_parameters;
ParameterMap Trigger::default_parameters;
ParameterMap Door::default_parameters;
ParameterMap Exit::default_parameters;
ParameterMap Teleporter::default_parameters;
ParameterMap Wall::default_parameters;
ParameterMap Water::default_parameters;
ParameterMap Wind::default_parameters;
ParameterMap Geyser::default_parameters;
ParameterMap Spike::default_parameters;

ParameterMap Item::default_parameters;
ParameterMap Heart::default_parameters;
ParameterMap Key::default_parameters;
ParameterMap Bomb::default_parameters;

ParameterMap Character::default_parameters;
ParameterMap DeadPlayer::default_parameters;
ParameterMap TriggeredBomb::default_parameters;

ParameterMap Monster::default_parameters;
ParameterMap Plant::default_parameters;
ParameterMap Zombie::default_parameters;

ParameterMap Player::default_parameters;
ParameterMap Erik::default_parameters;
ParameterMap Olaf::default_parameters;
ParameterMap Scorch::default_parameters;
ParameterMap Fang::default_parameters;
ParameterMap Baleog::default_parameters;

bool Compare::operator()(const Object* obj1, const Object* obj2) {
    if (obj1->onum<obj2->onum) return true;
    else return false;
}

ObjectsPool::ObjectsPool():
  currentplayer(playerspool.begin()),
  au_switch(sndcache->loadWAV("newchar.wav")) {
    setDefaultObjParam();
}
ObjectsPool::~ObjectsPool() {
    object_iterator i=objectspool.begin();
    while (i!=objectspool.end()) i=removeObject(i);
}
void ObjectsPool::setDefaultObjParam() {
    Object::default_parameters["name"]="";
    Object::default_parameters["w"]="";
    Object::default_parameters["h"]="";
    Object::default_parameters["anim"]="";
    Object::default_parameters["image"]="";
    Background::default_parameters=Object::default_parameters;
    Exit::default_parameters=Object::default_parameters;
    Exit::default_parameters["image"]="exit.bmp";
    Wall::default_parameters=Object::default_parameters;
    Door::default_parameters=Object::default_parameters;
    Door::default_parameters["key"]="Key";
    Door::default_parameters["start_open"]="";
    Door::default_parameters["audio_open"]="dooropn.wav";
    Door::default_parameters["audio_close"]="dooropn.wav";
    Teleporter::default_parameters=Object::default_parameters;
    Teleporter::default_parameters["exitx"]="0";
    Teleporter::default_parameters["exity"]="0";
    Teleporter::default_parameters["audio_teleport"]="teleprt.wav";
    Water::default_parameters=Object::default_parameters;
    Water::default_parameters["audio_water_land"]="splash2.wav";
    Wind::default_parameters=Object::default_parameters;
    Wind::default_parameters["accel"]="0";
    Trigger::default_parameters=Object::default_parameters;
    Trigger::default_parameters["image"]="key.bmp";
    Trigger::default_parameters["key"]="Key";
    Trigger::default_parameters["target"]="Target";
    Geyser::default_parameters=Object::default_parameters;
    Geyser::default_parameters["aspeed"]="0";
    Spike::default_parameters=Object::default_parameters;
    Spike::default_parameters["direction"]="up";

    Item::default_parameters=Object::default_parameters;
    Heart::default_parameters=Item::default_parameters;
    Heart::default_parameters["image"]="heart.bmp";
    Heart::default_parameters["alife"]="1";
    Key::default_parameters=Item::default_parameters;
    Key::default_parameters["image"]="key.bmp";
    Key::default_parameters["audio_key"]="";
    Bomb::default_parameters=Item::default_parameters;
    Bomb::default_parameters["image"]="bomb_fire.bmp";
    Bomb::default_parameters["image_bomb"]="bomb_fire.bmp";
    Bomb::default_parameters["countdown"]="3000";
    Bomb::default_parameters["audio_bomb"]="explsn.wav";

    Character::default_parameters=Object::default_parameters;
    Character::default_parameters["maxhealth"]=1;
    Character::default_parameters["health"]="1";
    Character::default_parameters["maxspeedx"]="300";
    Character::default_parameters["maxspeedy"]="0";
    DeadPlayer::default_parameters=Character::default_parameters;
    TriggeredBomb::default_parameters=Character::default_parameters;
    TriggeredBomb::default_parameters["image"]="bomb_fire.bmp";
    TriggeredBomb::default_parameters["countdown"]="3000";
    TriggeredBomb::default_parameters["audio_bomb"]="explsn.wav";

    Monster::default_parameters=Character::default_parameters;
    Monster::default_parameters["maxhealth"]=1;
    Monster::default_parameters["health"]="1";
    Monster::default_parameters["maxspeedx"]="50";
    Monster::default_parameters["maxspeedy"]="0";
    Monster::default_parameters["audio_hit"]="monhit.wav";
    Plant::default_parameters=Character::default_parameters;
    Plant::default_parameters["audio_hit"]="creeper.wav";
    Plant::default_parameters["audio_recover"]="creeper.wav";
    Plant::default_parameters["time_recover"]="3000";
    Zombie::default_parameters=Character::default_parameters;
    Zombie::default_parameters["maxspeedx"]="80";
    Zombie::default_parameters["audio_attack"]="clang.wav";

    Player::default_parameters=Character::default_parameters;
    Player::default_parameters["maxhealth"]="4";
    Player::default_parameters["health"]="3";
    Player::default_parameters["maxspeedx"]="300";
    Player::default_parameters["maxspeedy"]="200";
    Erik::default_parameters=Player::default_parameters;
    Olaf::default_parameters=Player::default_parameters;
    Scorch::default_parameters=Player::default_parameters;
    Fang::default_parameters=Player::default_parameters;
    Baleog::default_parameters=Player::default_parameters;
}

const ParameterMap& ObjectsPool::getDefaultObjParambyName(const string& obj) {
    if      (obj=="Object")          return Object::default_parameters;
    else if (obj=="Background")      return Background::default_parameters;
    else if (obj=="Trigger")         return Trigger::default_parameters;
    else if (obj=="Door")            return Door::default_parameters;
    else if (obj=="Exit")            return Exit::default_parameters;
    else if (obj=="Teleporter")      return Teleporter::default_parameters;
    else if (obj=="Wall")            return Wall::default_parameters;
    else if (obj=="Water")           return Water::default_parameters;
    else if (obj=="Wind")            return Wind::default_parameters;
    else if (obj=="Spike")           return Spike::default_parameters;

    else if (obj=="Item")            return Item::default_parameters;
    else if (obj=="Heart")           return Heart::default_parameters;
    else if (obj=="Key")             return Key::default_parameters;
    else if (obj=="Bomb")            return Bomb::default_parameters;

    else if (obj=="Character")       return Character::default_parameters;
    else if (obj=="DeadPlayer")      return DeadPlayer::default_parameters;
    else if (obj=="TriggeredBomb")   return TriggeredBomb::default_parameters;
    else if (obj=="Plant")           return Plant::default_parameters;
    else if (obj=="Zombie")          return Zombie::default_parameters;

    else if (obj=="Player")          return Player::default_parameters;
    else if (obj=="Erik")            return Erik::default_parameters;
    else if (obj=="Olaf")            return Olaf::default_parameters;
    else if (obj=="Scorch")          return Scorch::default_parameters;
    else if (obj=="Fang")            return Fang::default_parameters;
    else if (obj=="Baleog")          return Baleog::default_parameters;
    else return empty_parameter;
}

Object* ObjectsPool::addObjectbyName(const string& obj, Sint16 x, Sint16 y, ParameterMap& parameters) {
    //Set names...
    if (!hasParam(parameters,"name")) parameters["name"]=getNextObjectName(obj);

    if      (obj=="Wall")           return (addObject(new Wall(x,y,parameters)));
    else if (obj=="Exit")           return (addObject(new Exit(x,y,parameters)));
    else if (obj=="Water")          return (addObject(new Water(x,y,parameters)));
    else if (obj=="Teleporter")     return (addObject(new Teleporter(x,y,parameters)));
    else if (obj=="Wind")           return (addObject(new Wind(x,y,parameters)));
    else if (obj=="Geyser")         return (addObject(new Geyser(x,y,parameters)));
    else if (obj=="Trigger")        return (addObject(new Trigger(x,y,parameters)));
    else if (obj=="Door")           return (addObject(new Door(x,y,parameters)));
    else if (obj=="Spike")          return (addObject(new Spike(x,y,parameters)));
    else if (obj=="Heart")          return (addObject(new Heart(x,y,parameters)));
    else if (obj=="Key")            return (addObject(new Key(x,y,parameters)));
    else if (obj=="Bomb")           return (addObject(new Bomb(x,y,parameters)));
    else if (obj=="TriggeredBomb")  return (addCharacter(new TriggeredBomb(x,y,parameters)));
    else if (obj=="Erik")           return (addPlayer(new Erik(x,y,parameters)));
    else if (obj=="Olaf")           return (addPlayer(new Olaf(x,y,parameters)));
    else if (obj=="Baleog")         return (addPlayer(new Baleog(x,y,parameters)));
    else if (obj=="Fang")           return (addPlayer(new Fang(x,y,parameters)));  
    else if (obj=="Scorch")         return (addPlayer(new Scorch(x,y,parameters)));
    else if (obj=="Plant")          return (addMonster(new Plant(x,y,parameters)));
    else if (obj=="Zombie")         return (addMonster(new Zombie(x,y,parameters)));
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
