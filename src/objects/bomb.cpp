#include "common.h"
//shouldn't be here...
#include "objectpools.h"
#include "players_common.h"
#include "triggered_bomb.h"
#include "bomb.h"

//TODO: this adds a new object (TriggeredBomb), how to handle this with dynamical plugins?

Bomb::Bomb(string imagename, Uint16 xcord, Uint16 ycord, Uint16 explode_time, string iname):
  Item(imagename,xcord,ycord,iname),
  countdown(explode_time) { }
Bomb::~Bomb() { }

bool Bomb::act(Object*) {
    if (pool->addCharacter(new TriggeredBomb("bomb_fire.bmp",owner->getPos()->x,owner->getPos()->y,countdown,"TriggeredBomb"))) {
        owner->removeItem();
        return true;
    } else {
        return false;
    }
}

