#include "lost_vikings.h"
#include "events.h"

using namespace std;


//VIKING (Character)
Viking::Viking(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Character(imagename,xcord,ycord,vname) {
    health=3;
    maxspeedx=300;
    maxspeedy=200;
    for (Uint8 i=0; i<MAX_ITEMS; i++) {
        items[i]=NULL;
    }
    currentitem=0;
    state=STATE_FALL;
    otype|=OTYPE_VIKING;
    dense_types|=OTYPE_MONSTER;
    enemy_types|=OTYPE_MONSTER;
    t_water=500;
    im_left=animation;
    im_right=animation;
    im_fall_left=animation;
    im_fall_right=animation;
    im_krit_left=animation;
    im_krit_right=animation;
    im_land_left=animation;
    im_land_right=animation;
    au_land=NULL;
    au_act=loadWAV("button.wav");
}

Viking::~Viking() {
    for (Uint8 i=0; i<MAX_ITEMS; i++) {
        if (items[i]) {
            removeItem(i);
        }
    }
    if (au_land) Mix_FreeChunk(au_land);
    if (au_act) Mix_FreeChunk(au_act);
}

//updates the entered based states and values (includes updateTouch)
void Viking::addEnter(std::set<Object *>& aset) {
    Character::addEnter(aset);
    object_iterator obit=aset.begin();
    while (obit!=aset.end()) {
        if ((*obit)->getType()&OTYPE_ITEM) {
            if (Item* ptr = dynamic_cast<Item *>(*obit)) {
                pickupItem(ptr);
            }
        }
        ++obit;
    }
}

//updates the touched based states and values
void Viking::addTouch(std::set<Object *>& aset) {
    Character::addTouch(aset);
}
void Viking::removeEnter(std::set<Object *>& rset) {
    Character::removeEnter(rset);
}
void Viking::removeTouch(std::set<Object *>& rset) {
    Character::removeTouch(rset);
}

bool Viking::pickupItem(Item* newitem) {
    if (newitem==NULL) return false;
    for (Uint8 i=0; i<MAX_ITEMS; i++) {
        if (items[i] == NULL) {
            newitem->setOwner(this);
            newitem->setPos(0,0);
            pool->moveObject(newitem);
            items[i]=newitem;
            return true;
        }
    }
    return false;
}

void Viking::removeItem(Uint8 num) {
    if (num>=MAX_ITEMS || items[num]==NULL) return;
    delete items[num];
    items[num]=NULL;
}

Item* Viking::moveItem(Uint8 num) {
    if (num>=MAX_ITEMS || items[num]==NULL) return NULL;
    Item* tmpit=items[num];
    items[num]=NULL;
    return tmpit;
}

void Viking::switchItem(bool right) {
    if (right) {
        ++currentitem;
        if (currentitem>=MAX_ITEMS) currentitem=0;
    } else {
        if (currentitem>0) --currentitem;
        else currentitem=(MAX_ITEMS-1);
    }
}

Object* Viking::dropItem(Uint8 num, bool right) {
    Item* tmpit=moveItem(num);
    Object* tmpobj=NULL;
    bool ok=false;
    if (tmpit == NULL) return NULL;
    //check right/left according to (bool right)
    if (right) ok=tmpit->setPos(pos.x + pos.w + 1, Uint16(getCenter().y-tmpit->getPos()->h/2));
    else ok=tmpit->setPos(pos.x - tmpit->getPos()->w - 1, Uint16(getCenter().y-tmpit->getPos()->h/2));
    
    //new position in map and valid
    if (ok && ((tmpobj=pool->addObject(tmpit))!=NULL)) return tmpobj;
    else { 
        pickupItem(tmpit);
        return NULL;
    }
}

//check the states and change the image correspondingly
//TODO: add left, right, up, down, etc movement animations
void Viking::updateAnimState() {
    if (state&STATE_LEFT) {
        if (state&STATE_IRR) {
            animation=im_land_left;
        } else if (state&STATE_FALL) {
            if (state&STATE_FALL2) {
                animation=im_krit_left;
            } else {
                animation=im_fall_left;
            }
        } else {
            animation=im_left;
        }
    } else {
        if (state&STATE_IRR) {
            animation=im_land_right;
        } else if (state&STATE_FALL) {
            if (state&STATE_FALL2) {
                animation=im_krit_right;
            } else {
                animation=im_fall_right;
            }
        } else {
            animation=im_right;
        }
    }
    curpos.w=animation->getWidth();
    curpos.h=animation->getHeight();
    curpos.x=(Uint16)((pos.w-curpos.w)/2);
    curpos.y=(pos.h-curpos.h);
}

//first thing to do
void Viking::idle(Uint16 dt) {
    Character::idle(dt);
}

//input: activation key (return)
void Viking::in_act(Uint16) {
    input->unsetState(INPUT_ACT);
    object_iterator i=enter.begin();
    while (i!=enter.end()) {
        if ((*i)->act(this)) {
            sfxeng->playWAV(au_act);
            return;
        }
        ++i;
    }
}

//input: use key (insert)
void Viking::in_use(Uint16) {
    input->unsetState(INPUT_USE);
    if (items[currentitem]) items[currentitem]->act(this);
}

//input: right key
void Viking::in_right(Uint16) {
    unsetState(STATE_LEFT);
    setState(STATE_MRIGHT);
    hspeed+=maxspeedx;
}

//input: left key
void Viking::in_left(Uint16) {
    setState(STATE_LEFT);
    setState(STATE_MLEFT);
    hspeed-=maxspeedx;
}
//input: up key
void Viking::in_up(Uint16) { }
//input: down key
void Viking::in_down(Uint16) { }
//input: special1 key (SPACE)
void Viking::in_sp1(Uint16) { }
//input: special2 key (SHIFT)
void Viking::in_sp2(Uint16) { }

//moving (help function)
Hit Viking::move(Uint16 dt, bool check) {
    return Character::move(dt,check);
}


//falling
void Viking::fall(Uint16 dt) {
    Character::fall(dt);
}

//landing
void Viking::land() {
    Character::land();
    unsetState(STATE_ACT_1);
    unsetState(STATE_ACT_2);

    if (state&STATE_FALL2) {
        unsetState(STATE_FALL2);
        setEvent(new ELand(this,T_IRR,au_land));
    }
}

//dying
void Viking::die() {
    failed=true;
    Character::die();
}
