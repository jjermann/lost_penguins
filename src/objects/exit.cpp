#include "common.h"
#include "objectpools.h"
//shouldn't be here...
#include "players_common.h"
#include "exit.h"


Exit::Exit(string imagename, Sint16 xcord, Sint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname) { }
Exit::~Exit() { }

bool Exit::act(Object*) {
    player_iterator it=pool->playerspool.begin();
    while (it!=pool->playerspool.end()) {
        if (!((*it)->isIn(pos))) return false;
        ++it;
    }
    if (failed) cout << "Again?\n"; 
    else cout << "You won!!!!\n";
    quitGame(0);
    return true;
}
