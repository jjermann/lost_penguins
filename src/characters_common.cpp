#include "lost_vikings.h"
#include "weapons.h"
#include "events.h"

using namespace std;


//CHARACTER (Object)
Character::Character(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Object(imagename,xcord,ycord,vname) {
    health=1;
    maxspeedx=300;
    maxspeedy=0;
    speed=hspeed=0;
    gravity=900;
    Dgrav=0;
    state=STATE_FALL;
    otype|=OTYPE_CHARACTER;
    t_water=0;
    im_orig=animation;
    dense_types=NOTHING;
    weapon=Weapon(0);
}

Character::~Character() { }

//add a set of regions to the enter set
void Character::addEnter(std::set<Object *>& aset) {
    object_iterator obit=aset.begin();
    while (obit != aset.end()) {
        (*obit)->enter(this);
        ++obit;
    }
}

//add a set of regions to the touch set
void Character::addTouch(std::set<Object *>& aset) {
    object_iterator obit=aset.begin();
    while (obit != aset.end()) {
        (*obit)->touch(this);
        ++obit;
    }
}

//remove a set of regions from the enter set
void Character::removeEnter(std::set<Object *>& rset) {
    object_iterator obit=rset.begin();
    while (obit != rset.end()) {
        (*obit)->leave(this);
        ++obit;
    }
}

//remove a set of regions from the touch set
void Character::removeTouch(std::set<Object *>& rset) {
    object_iterator obit=rset.begin();
    while (obit != rset.end()) {
        (*obit)->untouch(this);
        ++obit;
    }
}

void Character::removedObject(Object* obj) {
    touch.erase(obj);
    enter.erase(obj);
}

//check the states and change the image correspondingly
//TODO: add left, right, up, down, etc movement animations
void Character::updateAnimState() { }

//first thing to do
void Character::idle(Uint16 dt) {
    //check if we have ground below us...
    Hit hit;
    hit=checkMove(pos);

    //can't swim
    if(state&STATE_WATER) {
        Dwater+=dt;
        if (Dwater>=t_water) { 
            addHealth(-1);
            Dwater=0;
        }
    } else Dwater=0;

    //are we falling?
    if ((!(hit.touch&DIR_DOWN)) || (gravity<0)) {
        setState(STATE_FALL);
    }
}

//moving (help function)
Hit Character::move(Uint16 dt, bool check) {
    //checks if we already have a critical speed
    //add accelerations for each viking in viking.cpp!
    if (!check) {
        if (state&STATE_FALL) {
            if (speed>V_KRIT) {
                speed=V_KRIT;
                setState(STATE_FALL2);
            } else unsetState(STATE_FALL2);
        }
    }

    SDL_Rect dest=pos;
    dest.x+=Sint16((hspeed*dt)/1000);
    dest.y+=Sint16((speed*dt)/1000);
    
    Hit hit=checkMove(dest,check);
    
    if (!check) {
        //temporary changes (changes hspeed)
        pos.x=dest.x; 
        pos.y=dest.y; 

        if ((hit.enter&DIR_LEFT) || (hit.enter&DIR_RIGHT)) {
            if (event) event->cancel();
            unsetState(STATE_MLEFT);
            unsetState(STATE_MRIGHT);
            hspeed=0;     
        }
        if (hit.enter&DIR_UP) {
            if (event) event->cancel();
            speed=0;
        }
    }
    
    return hit;
}


//falling
void Character::fall(Uint16 dt) {
    Dgrav+=dt;  
    if (Dgrav>T_GRAV_EFFECT) {
        if (state&STATE_FALL) {
            addSpeed(Sint16(gravity*Dgrav/1000));
        }
        Hit hit=move(Dgrav);
        if ((hit.enter&DIR_DOWN || hit.touch&DIR_DOWN) && (state&STATE_FALL)) land();
        Dgrav=0;
    }
}

//landing
void Character::land() {
    if (event) event->cancel();
    speed=0;
    unsetState(STATE_FALL);
    Dgrav=0;
}

void Character::die() {
    mark_delete();

    character_iterator cit=pool->characterspool.begin();
    while (cit != pool->characterspool.end()) {
        (*cit)->removedObject(this);
        ++cit;
    }
}

Uint8 Character::getHealth() {
    return health;
}

//PRE:  new health of viking
//ACT:  changes health, deletes object if health=0
//POST: returns new health
Uint8 Character::setHealth(Uint8 newhealth) {
    if (newhealth==0) {
        cout << name << " died!\n";
        health=0;
        die();
        return (health);
    } else if (newhealth > MAX_HEALTH) {
        return (health=MAX_HEALTH);
    } else {
        return (health=newhealth);
    }
}

Uint8 Character::addHealth(Sint8 dhealth) {
    if ((health+dhealth)>=0) return setHealth(health+dhealth);
    else return health;
}

//PRE:  movement (to dest), target object (pointer) destobj, the character itself should
//      _not_ be hitting. tele (default false) deactivates a direction check and checks
//      all directions
//ACT:  checks the type of impact and touch
//POST: hit type of movement into destobj, if this==destobj or destobj==NULL: HIT_NOTHING
inline Hit Character::checkHit(const SDL_Rect& dest, Object* destobj, bool tele) const {
    //Initialize
    Hit hit;
    hit.enter=NOTHING;
    hit.touch=NOTHING;
    bool entered=false;

    //can't hit nothing
    if (destobj==NULL) return hit;
    //Objects don't hit themself...
    if (this==destobj) return hit;

    SDL_Rect* opos=destobj->getPos();
    
    //dest not contained in destobj? (no hit)
    if ( ((dest.x+dest.w)<opos->x)  || (dest.x>(opos->x+opos->w)) ||
      (dest.y>(opos->y+opos->h)) || ((dest.y+dest.h)<opos->y) ) {
        return hit;
    }

    Hit hitdir;
    hitdir.enter=DIR_ALL;
    hitdir.touch=NOTHING;
    
    //touching
    if ((dest.x+dest.w)==opos->x) hitdir.touch|=DIR_RIGHT;
    if (dest.x==(opos->x+opos->w)) hitdir.touch|=DIR_LEFT;
    if (dest.y==(opos->y+opos->h)) hitdir.touch|=DIR_UP;
    if ((dest.y+dest.h)==opos->y) hitdir.touch|=DIR_DOWN;
    if (hitdir.touch!=NOTHING) hitdir.enter=NOTHING;

    if ((hitdir.touch&DIR_DWR) == DIR_DWR) hitdir.touch&=~DIR_DWR;
    if ((hitdir.touch&DIR_UPR) == DIR_UPR) hitdir.touch&=~DIR_UPR;
    if ((hitdir.touch&DIR_DWL) == DIR_DWL) hitdir.touch&=~DIR_DWL;
    if ((hitdir.touch&DIR_UPL) == DIR_UPL) hitdir.touch&=~DIR_UPL;

    //=== From this point on general hitting directions are set ===//
    
    //Did we enter something?
    if (hitdir.enter) entered=true;

    //plausibility checks (!tele assumption: this didn't hit the object)
    if (!tele) {
        if ((pos.x+pos.w)>opos->x) hitdir.enter&=~DIR_RIGHT;
        if (pos.x<(opos->x+opos->w)) hitdir.enter&=~DIR_LEFT;
        if (pos.y<(opos->y+opos->h)) hitdir.enter&=~DIR_UP;
        if ((pos.y+pos.h)>opos->y) hitdir.enter&=~DIR_DOWN;
        hitdir.enter&=curmap->getDirection(pos,dest);
    }

    //=== From this point on movement hit directions are set ===//
    
    //dense hit (used a trick to write it in a compact form):
    hit.enter=destobj->getType();
    hit.touch=destobj->getType();
    if (hit.enter&dense_types) hit.enter|=DIR_LR;
    hit.enter&=~(DIR_ALL&(~hitdir.enter));
    if (hit.touch&dense_types) hit.touch|=DIR_LR;
    hit.touch&=~(DIR_ALL&(~hitdir.touch));
 
    if (entered) {
        if ( (dest.x+Uint16(dest.w/2)) < (opos->x+opos->w) &&
          (dest.x+Uint16(dest.w/2)) > opos->x &&
          (dest.y+Uint16(dest.h/2)) < (opos->y+opos->h) &&
          (dest.y+Uint16(dest.h/2)) > opos->y ) {
            hit.enter|=OTYPE_ENTER;
        } else {
            hit.enter|=OTYPE_TOUCH;
        }
    } else if (hitdir.touch) {
        hit.touch|=OTYPE_TOUCH;
    }

   return hit;
}

//PRE:  movement (to dest) with checkHit() ok
//      tele (default false) deactivates a direction check and checks all direction
//ACT:  checks if the movement hits an object or map boundary, changes the destination respectively
//      updates the entered/touched objects of obj
//POST: directions of a collision/touching (type HIT_DENSE + map boundaries), NOTHING if the obj pointer is NULL
Hit Character::checkMove(SDL_Rect& dest, bool tele, bool check) {
    Hit colltype;
    colltype.enter=NOTHING;
    colltype.touch=NOTHING;
    SDL_Rect opos;
    SDL_Rect olddest=dest;
    Hit hit;
    std::set<Object *> newtouch;
    std::set<Object *> newenter;
    
    //Change destination based on enter hits on normal objects
    object_iterator obit=pool->objectspool.begin();
    while (obit!=pool->objectspool.end()) {
        //blocked directions == movement direction in this case
        if (!((*obit)->isDeleted())) {
            opos=*((*obit)->getPos());
            //the order here is important!
            hit=checkHit(dest,(*obit));
            colltype.enter|=hit.enter;
            if(hit.enter&DIR_UP) dest.y=opos.y+opos.h;
            if(hit.enter&DIR_DOWN) dest.y=opos.y-dest.h;
            if(hit.enter&DIR_RIGHT) dest.x=opos.x-dest.w;
            if(hit.enter&DIR_LEFT) dest.x=opos.x+opos.w;
        }
        ++obit;
    }

    //Check map boundary (i=0)
    hit=curmap->checkPlace(dest,(*maparea));
    if (hit.enter&DIR_RIGHT) {
        colltype.enter|=DIR_RIGHT;
        colltype.touch|=DIR_RIGHT;
        dest.x=maparea->x+maparea->w-dest.w;
    } else if (hit.touch&DIR_RIGHT) {
        colltype.touch|=DIR_RIGHT;
    }
    if (hit.enter&DIR_LEFT) {
        colltype.enter|=DIR_LEFT;
        colltype.touch|=DIR_LEFT;
        dest.x=maparea->x;
    } else if (hit.touch&DIR_LEFT) {
        colltype.touch|=DIR_LEFT;
    }
    //no DIR_UP as we want the upper part to be unbounded
    if (hit.enter&DIR_DOWN) {
        colltype.enter|=DIR_DOWN;
        colltype.touch|=DIR_DOWN;
       dest.y=maparea->y+maparea->h-dest.h;
    } else if (hit.touch&DIR_DOWN) {
        colltype.touch|=DIR_DOWN;
    }

    //=== From this point on the new destination is calculated ===//
    
    //check touching
    obit=pool->objectspool.begin();
    while (obit!=pool->objectspool.end()) {
        //blocked directions == movement direction in this case
        if (!((*obit)->isDeleted())) {
            hit=checkHit(dest,(*obit));
            colltype.touch|=hit.touch;
            
            if (hit.enter&OTYPE_TOUCH || hit.touch) newtouch.insert(*obit);
            if ((hit.enter&OTYPE_ENTER)==OTYPE_ENTER) newenter.insert(*obit);
        }
        ++obit;
    }
    
    if (!check) updateRegions(newtouch,newenter);
    else dest=olddest;

    //shouldn't happen
    if ( (!tele) && (((colltype.enter&DIR_RIGHT) && (colltype.enter&DIR_LEFT)) || ((colltype.enter&DIR_UP) && (colltype.enter&DIR_DOWN))) ) {
        colltype.enter=NOTHING;
    }
    return colltype;
}


inline void Character::updateRegions(std::set<Object *>& newtouch, std::set<Object *>& newenter) {
    //new touching objects
    std::set<Object *> addtouch;
    set_difference(newtouch.begin(),newtouch.end(),touch.begin(),touch.end(),inserter(addtouch,addtouch.begin()));
    //new entering objects
    std::set<Object *> remtouch;
    set_difference(touch.begin(),touch.end(),newtouch.begin(),newtouch.end(),inserter(remtouch,remtouch.begin()));
    //untouching + removed objects
    std::set<Object *> addenter;
    set_difference(newenter.begin(),newenter.end(),enter.begin(),enter.end(),inserter(addenter,addenter.begin()));
    //leaving + removed objects
    std::set<Object *> rementer;
    set_difference(enter.begin(),enter.end(),newenter.begin(),newenter.end(),inserter(rementer,rementer.begin()));
    
    //change sets
    enter.swap(newenter);
    touch.swap(newtouch);

    //remove old elements
    if (!remtouch.empty()) removeTouch(remtouch);
    if (!rementer.empty()) removeEnter(rementer);
    //add new elements
    if (!addtouch.empty()) addTouch(addtouch);
    if (!addenter.empty()) addEnter(addenter);
}


//gets hit by a weapon
Sint16 Character::hit(Uint16 dir, Weapon& weap) {
    Sint16 dhealth=addHealth(weap.getDamage());
    switch (weap.getType()) {
        case W_STRIKE: {
            if (dir&DIR_RIGHT) setEvent(new ESpeed(this,TSTRIKE,-60,DSTRIKE,0,0,NULL));
            if (dir&DIR_LEFT)  setEvent(new ESpeed(this,TSTRIKE,-60,-DSTRIKE,0,0,NULL));
            break;
        }
        default: {
            break;
        }
    }
    return dhealth;
}
