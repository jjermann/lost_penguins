#include "common.h"
#include "objectpools.h"
//shouldn't be here...
#include "vikings_common.h"
#include "exit.h"


Exit::Exit(string imagename, Uint16 xcord, Uint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname) { }
Exit::~Exit() { }

bool Exit::act(Object*) {
    viking_iterator it=pool->vikingspool.begin();
    while (it!=pool->vikingspool.end()) {
        if (!((*it)->isIn(pos))) return false;
        ++it;
    }
    if (failed) cout << "Again?\n"; 
    else cout << "You won!!!!\n";
    quitGame(0);
    return true;
}
