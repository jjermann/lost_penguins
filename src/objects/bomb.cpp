#include "common.h"
//shouldn't be here...
#include "objectpools.h"
#include "scenario.h"
#include "players_common.h"
#include "triggered_bomb.h"
#include "bomb.h"

Bomb::Bomb(string imagename, Sint16 xcord, Sint16 ycord, Uint16 explode_time, string iname):
  Item(imagename,xcord,ycord,iname),
  countdown(explode_time) { }
Bomb::~Bomb() { }

bool Bomb::act(Object*) {
    if (scenario->pool->addCharacter(new TriggeredBomb("bomb_fire.bmp",owner->getPos()->x,owner->getPos()->y,countdown,"TriggeredBomb"))) {
        owner->removeItem();
        return true;
    } else {
        return false;
    }
}

