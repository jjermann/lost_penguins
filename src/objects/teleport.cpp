#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
//shouldn't be here...
#include "players_common.h"
#include "teleport.h"


//(xExit,yExit) are the destination coordinates of the teleporter (bottom center)
Teleporter::Teleporter(string imagename, Uint16 xcord, Uint16 ycord, Uint16 xExit, Uint16 yExit, string oname):
  Object(imagename,xcord,ycord,oname),
  au_tele(sndcache->loadWAV("teleprt.wav")) {
    exit.x=xExit;
    exit.y=yExit;
}
Teleporter::~Teleporter() { }

bool Teleporter::act(Object* obj) {
    SDL_Rect newpos=exit;
    //checks if obj is a player...
    if(Player* ptr = dynamic_cast<Player*>(obj)) {
        newpos.x-=(Uint16)((obj->getPos()->w)/2);
        newpos.y-=pos.h;
        //try teleporting (otherwise, the position stays the same)
        SDL_Rect testpos=newpos;
        if (((ptr->checkMove(newpos,true).enter)&DIR_ALL)==NOTHING) {
            sfxeng->playWAV(au_tele);
            obj->setPos(newpos.x, newpos.y);
            return true;
        } else {
            //TODO: catch this better
            cout << "Teleporter malfunction!!\n";
            sfxeng->playWAV(au_tele);
            obj->setPos(newpos.x, newpos.y);
            return true;
        }
    } else return false;
}
