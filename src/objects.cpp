#include "lost_vikings.h"
#include "objects.h"

using namespace std;

//BACKGROUND (Object)
Background::Background(string imagename):
  Object(imagename,0,0,"Background") {
    otype=NOTHING;
}
Background::~Background() { }

//WALL (Object)
Wall::Wall(string imagename, Uint16 xcord, Uint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname) {
    otype=OTYPE_DENSE;
}
Wall::~Wall() { }

//EXIT (Object)
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

//WATER (Object)
Water::Water(string imagename, Uint16 xcord, Uint16 ycord, string oname):
  Object(imagename,xcord,ycord,oname) { }
Water::~Water() { }

void Water::enter(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->setState(STATE_WATER);
    }
}

void Water::leave(Object* obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->unsetState(STATE_WATER);
    }
}


//WIND (Object)
Wind::Wind(string imagename, Uint16 xcord, Uint16 ycord, Sint16 xAccel, Sint16 yAccel, string oname):
  Object(imagename,xcord,ycord,oname) {
    gravitymod.x=xAccel;
    gravitymod.y=yAccel;
}
Wind::~Wind() { }

void Wind::enter(Object *obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->addGravity(gravitymod);
    }
}
void Wind::leave(Object *obj) {
    if(Character* ptr = dynamic_cast<Character*>(obj)) {
        ptr->addGravity(-gravitymod);
    }
}

//GEYSIR (Object)
Geysir::Geysir(string imagename, Uint16 xcord, Uint16 ycord, Sint16 xAdd, Sint16 yAdd, string oname):
  Object(imagename,xcord,ycord,oname) {
    aspeed.x=xAdd;
    aspeed.y=yAdd;
    au_geysir=NULL;
    Deffect=0;
}
Geysir::~Geysir() {
    if (au_geysir) Mix_FreeChunk(au_geysir);
}

void Geysir::idle(Uint16 dt) {
    Object::idle(dt);
    Deffect+=dt;
    if (Deffect>T_GEYSIR) {
        Deffect=0;
        sfxeng->playWAV(au_geysir);
        std::set<Viking *> vikings=map->getVikingsIn(pos,true);
        std::set<Viking *>::iterator vkit=vikings.begin();
        while (vkit!=vikings.end()) {
            (*vkit)->addSpeed(aspeed);
            ++vkit;
        }
    }
}


//TELEPORTER (Object)
//(xExit,yExit) are the destination coordinates of the teleporter (bottom center)
Teleporter::Teleporter(string imagename, Uint16 xcord, Uint16 ycord, Uint16 xExit, Uint16 yExit, string oname):
  Object(imagename,xcord,ycord,oname) {
    exit.x=xExit;
    exit.y=yExit;
    au_tele=loadWAV("tele.wav");
}
Teleporter::~Teleporter() {
    if (au_tele) Mix_FreeChunk(au_tele);
}

bool Teleporter::act(Object* obj) {
    SDL_Rect newpos=exit;
    //checks if obj is a viking...
    if(Viking* ptr = dynamic_cast<Viking*>(obj)) {
        sfxeng->playWAV(au_tele);
        newpos.x-=(Uint16)((obj->getPos()->w)/2);
        newpos.y-=pos.h;
        //try teleporting (otherwise, the position stays the same)
        if (ptr->checkMove(newpos,true).enter==NOTHING) obj->setPos(newpos.x, newpos.y);
        return true;
    } else return false;
}

//Trigger (Object)
Trigger::Trigger(string imagename, Uint16 xcord, Uint16 ycord, string targetname, string oname, string keyname):
  Object(imagename,xcord,ycord,oname) {
    target=pool->getObject(targetname);
    key=keyname;
}
Trigger::~Trigger() { }

bool Trigger::act(Object* obj) {
    //sanity check
    if (!(key.empty() || (obj && obj->getName()==key))) return false;

    if (target) {
        if (target->act(this)) return true;
        else return false;
    } else return false;
}

//Door (Object)
Door::Door(string imagename, Uint16 xcord, Uint16 ycord, string keyname, string oname):
  Object(imagename,xcord,ycord,oname) {
    open=false;
    otype=OTYPE_DENSE;
    key=keyname;
}
Door::~Door() { }

bool Door::act(Object* obj) {
    //sanity check
    if (!(obj && obj->getName()==key)) return false;
    if (!(map->getCharactersIn(pos,true).empty())) return false;

    //switch state
    if (open) open=false;
    else open=true;
    
    //TODO: check if closing is ok
    //open or close the door
    if (open) otype=NOTHING;
    else otype=OTYPE_DENSE;

    return true;
}


//HEART (Item)
Heart::Heart(string imagename, Uint16 xcord, Uint16 ycord, string iname):
  Item(imagename,xcord,ycord,iname) {
    au_heal=loadWAV("health.wav");
}
Heart::~Heart() {
    if (au_heal) Mix_FreeChunk(au_heal);
}

bool Heart::act(Object* obj) {
    if ((obj==NULL) || (obj!=owner)) return false;
    sfxeng->playWAV(au_heal);
    owner->addHealth(1);
    owner->removeItem();
    return true;
}

//KEY (Item)
Key::Key(string imagename, Uint16 xcord, Uint16 ycord, string iname):
  Item(imagename,xcord,ycord,iname) {
    au_key=NULL;
}
Key::~Key() {
    if (au_key) Mix_FreeChunk(au_key);
}

bool Key::act(Object* obj) {
    if ((obj==NULL) || (obj!=owner)) return false;
    std::set<Object *> enter=owner->getEnter();
    object_iterator i=enter.begin();
    while (i!=enter.end()) {
        if ((*i)->act(this)) {
            sfxeng->playWAV(au_key);
            owner->removeItem();
            return true;
        } else ++i;
    }
    return false;
}


//BOMB (Item)
Bomb::Bomb(string imagename, Uint16 xcord, Uint16 ycord, Uint16 explode_time, string iname):
  Item(imagename,xcord,ycord,iname), countdown(explode_time) { }
Bomb::~Bomb() { }

bool Bomb::act(Object*) {
    if (pool->addCharacter(new TriggeredBomb("bomb_fire.bmp",owner->getPos()->x,owner->getPos()->y,countdown,"TriggeredBomb"))) {
        owner->removeItem();
        return true;
    } else {
        return false;
    }
}
