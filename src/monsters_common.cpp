#include "lost_vikings.h"

using namespace std;


/*==================
  Monster (Object)
==================*/

Monster::Monster(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Character(imagename,xcord,ycord,vname) {
    health=1;
    maxspeedx=50;
    maxspeedy=0;
    state=STATE_FALL;
    otype|=OTYPE_MONSTER;
    dense_types|=OTYPE_VIKING;
    enemy_types|=OTYPE_VIKING;
    t_water=1000;
    im_left=animation;
    im_right=animation;
    Dai=Dattack=0;
}

Monster::~Monster() { }

//updates the entered based states and values (includes updateTouch)
void Monster::addEnter(std::set<Object *>& aset) {
    Character::addEnter(aset);    
}

//updates the touched based states and values
void Monster::addTouch(std::set<Object *>& aset) {
    Character::addTouch(aset);
    Viking* ptr=NULL;
    object_iterator obit=aset.begin();
    //TODO: clean up this mess
    while (obit != aset.end()) {
        if ((*obit)->getType()&enemy_types) {
            if((ptr = dynamic_cast<Viking*>(*obit))) {
                setState(STATE_ATTACK);
                targets.insert(ptr);
            }
        }
        ++obit;
    }
}
void Monster::removeEnter(std::set<Object *>& rset) {
    Character::removeEnter(rset);
    unsetState(STATE_ATTACK);
}
void Monster::removeTouch(std::set<Object *>& rset) {
    Character::removeTouch(rset);
    Viking* ptr=NULL;
    object_iterator obit=rset.begin();
    while (obit != rset.end()) {
        if ((*obit)->getType()&OTYPE_VIKING) {
            if((ptr = dynamic_cast<Viking*>(*obit))) {
                targets.erase(ptr);
            }
        }
        ++obit;
    }

    if (targets.empty()) unsetState(STATE_ATTACK);
}

void Monster::removedObject(Object* obj) {
    Character::removedObject(obj);
    if (Viking* ptr = dynamic_cast<Viking*>(obj)) {
        targets.erase(ptr);
    }
}

//check the states and change the image correspondingly
//TODO: add left, right, up, down, etc movement animations
void Monster::updateAnimState(bool change) {
    if (!change) {
    } else if (state&STATE_LEFT) {
        animation=im_left;   
    } else {   
        animation=im_right;
    }
    curpos.w=animation->getWidth();
    curpos.h=animation->getHeight();   
    curpos.x=(Uint16)((pos.w-curpos.w)/2);
    curpos.y=(pos.h-curpos.h);
}

//first thing to do
void Monster::idle(Uint16 dt) {
    Character::idle(dt);
    runAI(dt);
}

//moving (help function)
Hit Monster::move(Uint16 dt, bool check) {
    return Character::move(dt,check);
}


//falling
void Monster::fall(Uint16 dt) {
    Character::fall(dt);
}

//landing
void Monster::land() {
    Character::land();
}

//dying
void Monster::die() {
    Character::die();
}

Uint16 Monster::hit(Uint16 dir, Weapon& weap) {
    return Character::hit(dir,weap);
}
