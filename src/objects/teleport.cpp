#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
//shouldn't be here...
#include "players_common.h"
#include "teleport.h"


//(xExit,yExit) are the destination coordinates of the teleporter (bottom center)
Teleporter::Teleporter(string imagename, Uint16 xcord, Uint16 ycord, Uint16 xExit, Uint16 yExit, string oname):
  Object(imagename,xcord,ycord,oname) {
    exit.x=xExit;
    exit.y=yExit;
    au_tele=sndcache->loadWAV("teleprt.wav");
}
Teleporter::~Teleporter() { }

bool Teleporter::act(Object* obj) {
    SDL_Rect newpos=exit;
    //checks if obj is a player...
    if(Player* ptr = dynamic_cast<Player*>(obj)) {
        sfxeng->playWAV(au_tele);
        newpos.x-=(Uint16)((obj->getPos()->w)/2);
        newpos.y-=pos.h;
        //try teleporting (otherwise, the position stays the same)
        if (ptr->checkMove(newpos,true).enter==NOTHING) obj->setPos(newpos.x, newpos.y);
        return true;
    } else return false;
}
