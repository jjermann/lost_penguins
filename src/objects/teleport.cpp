#include "common.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "objects_common.h"
//shouldn't be here...
#include "players_common.h"
#include "scenario.h"
#include "teleport.h"


Teleporter::Teleporter(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Object(xcord,ycord,parameters) {
    /* Parameters */
    if (hasParam(parameters,"audio_teleport")) au_tele=scenario->sndcache->loadWAV(parameters["audio_teleport"]);
      else au_tele=scenario->sndcache->loadWAV("teleprt.wav");
    if (hasParam(parameters,"exitx")) exit.x=atoi(parameters["exitx"].c_str());
      else exit.x=0;
    if (hasParam(parameters,"exity")) exit.y=atoi(parameters["exity"].c_str());
      else exit.y=0;
    exit.w=exit.h=0;
}
Teleporter::~Teleporter() { }

bool Teleporter::act(Object* obj) {
    SDL_Rect newpos=exit;
    //checks if obj is a player...
    if(Player* ptr = dynamic_cast<Player*>(obj)) {
        newpos.x-=(Uint16)((obj->getPos()->w)/2);
        newpos.y-=pos.h;
        newpos.w=ptr->getPos()->w;
        newpos.h=ptr->getPos()->h;
        //try teleporting (otherwise, the position stays the same)
        SDL_Rect testpos=newpos;
        if (((ptr->checkMove(testpos,true,true).enter)&DIR_ALL)==NOTHING) {
            sfxeng->playWAV(au_tele);
            ptr->checkMove(newpos,true);
            if (!(obj->setPos(newpos.x, newpos.y))) cout << "Teleporter malfunction!!\n";
        } else {
            //TODO: catch this better
            cout << "Teleporter not working properly!\n";
        }
        return true;
    } else return false;
}
