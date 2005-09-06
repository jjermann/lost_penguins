#include "common.h"
#include "objectpools.h"
//shouldn't be here...
#include "scenario.h"
#include "players_common.h"
#include "exit.h"


Exit::Exit(string imagename, Sint16 xcord, Sint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname) { }
Exit::~Exit() { }

bool Exit::act(Object*) {
    player_iterator it=scenario->pool->playerspool.begin();
    while (it!=scenario->pool->playerspool.end()) {
        if (!((*it)->isIn(pos))) return false;
        ++it;
    }
    scenario->finnished=true;
    return true;
}
