#include "common.h"
#include "events.h"
#include "input.h"
#include "anim.h"
#include "weapons.h"
#include "sfxeng.h"
#include "imgcache.h"
#include "sndcache.h"
#include "objectpools.h"
#include "vikings_common.h"


//VIKING (Character)
Viking::Viking(string imagename, Uint16 xcord, Uint16 ycord, string vname):
  Character(imagename,xcord,ycord,vname) {
    health=3;
    maxhealth=4;
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
    au_land=sndcache->loadWAV("dizzy.wav");
    au_act=sndcache->loadWAV("button.wav");
    au_newitem=sndcache->loadWAV("pickup.wav");
    au_hit=sndcache->loadWAV("vikhit.wav");
    au_elec=sndcache->loadWAV("elecdth.wav");
    au_drown=sndcache->loadWAV("drown.wav");
    au_fire=sndcache->loadWAV("fireball.wav");
    au_die=sndcache->loadWAV("bones.wav");
    au_heal=sndcache->loadWAV("usefood1.wav");
}

Viking::~Viking() {
    for (Uint8 i=0; i<MAX_ITEMS; i++) {
        if (items[i]) {
            removeItem(i);
        }
    }
}

Frame Viking::getIcon() const {
    return (im_right->getFrame(0));
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
            sfxeng->playWAV(au_newitem);
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
void Viking::updateAnimState(bool change) {
    if (!change) {
    } else if (state&STATE_LEFT) {
        if (state&STATE_FALL) {
            if (state&STATE_FALL2) {
                animation=im_krit_left;
            } else {
                animation=im_fall_left;
            }
        } else {
            animation=im_left;
        }
    } else {
        if (state&STATE_FALL) {
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
void Viking::in_act(Sint16 dt) {
    if (dt < 0) return;
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
void Viking::in_use(Sint16 dt) {
    if (dt < 0) return;
    input->unsetState(INPUT_USE);
    if (items[currentitem]) items[currentitem]->act(this);
}

//input: right key
void Viking::in_right(Sint16 dt) {
    if (dt < 0) {
        if (state&STATE_MRIGHT) {
            unsetState(STATE_MRIGHT);
            hspeed-=maxspeedx;
        }
        return;
    }
    unsetState(STATE_LEFT);
    if (!(state&STATE_MRIGHT)) {
        hspeed+=maxspeedx;
        setState(STATE_MRIGHT);
    }
}

//input: left key
void Viking::in_left(Sint16 dt) {
    if (dt < 0) {
        if (state&STATE_MLEFT) {
            unsetState(STATE_MLEFT);
            hspeed+=maxspeedx;
        }
        return;
    }
    setState(STATE_LEFT);
    if (!(state&STATE_MLEFT)) {
        hspeed-=maxspeedx;
        setState(STATE_MLEFT);
    }
}
//input: up key
void Viking::in_up(Sint16) { }
//input: down key
void Viking::in_down(Sint16) { }
//input: special1 key (SPACE)
void Viking::in_sp1(Sint16) { }
//input: special2 key (SHIFT)
void Viking::in_sp2(Sint16) { }

Hit Viking::move(Uint16 dt, bool check) {
    return Character::move(dt,check);
}

void Viking::fall(Uint16 dt) {
    Character::fall(dt);
}

void Viking::crash(Uint16 dir) {
    Character::crash(dir);
    switch (dir) {
        case DIR_DOWN: {
            clearStates(true);

            if (state&STATE_FALL2) {
                unsetState(STATE_FALL2);
                Weapon tmpweap(-1,W_PRESSURE,WS_PRESSURE);
                hit(DIR_UP,tmpweap);
            }
            break;
        }
        default: { }
    }
}

void Viking::die() {
    failed=true;
    Character::die();
}

//unset all interactivity
void Viking::clearStates(bool reset) {
    if (getState(STATE_MLEFT)) in_left(-1);
    if (getState(STATE_MRIGHT)) in_right(-1);
    if (getState(STATE_MUP)) in_up(-1);
    if (getState(STATE_MDOWN)) in_down(-1);
    if (reset) {
        unsetState(STATE_ACT_1);
        unsetState(STATE_ACT_2);
        unsetState(STATE_ACT_3);
    }
}

Uint16 Viking::hit(Uint16 direction, Weapon& weap) {
    Uint16 newhealth;
    if (weap.getType()&W_WATER) newhealth=setHealth(0);
    else newhealth=addHealth(weap.getDamage());
    
    //died
    if (newhealth==0) {
        die();
        //should be !=NULL or sthg is wrong with the placement code...
        Character* deadvik=pool->addCharacter(new DeadViking("dead_viking.bmp",pos.x,pos.y));
        switch(weap.getSubType()) {
            case WS_FIRE: {
                if (deadvik && im_die) deadvik->setEvent(new EAnim(deadvik,im_die,false,0,ESTATE_BUSY,au_fire));
                else sfxeng->playWAV(au_fire);
                break;
            }
            case WS_WATER: {
                if (deadvik && im_die) deadvik->setEvent(new EAnim(deadvik,im_die,false,0,ESTATE_BUSY,au_drown));
                else sfxeng->playWAV(au_drown);
                break;
            }
            //WS_NORMAL, WS_ELECTRIC, WS_FIRE, WS_PRESSURE
            default: {
                if (deadvik && im_die) deadvik->setEvent(new EAnim(deadvik,im_die,false,0,ESTATE_BUSY,au_die));
                else sfxeng->playWAV(au_die);
            }
        }
    //damaged or healed...
    } else {
        Mix_Chunk* aud_hit;
        Animation* anim_hit;
        if (state&STATE_LEFT) anim_hit=im_land_left;
        else anim_hit=im_land_right;
        //TODO add more Animations
        switch(weap.getSubType()) {
            case WS_HEAL: {
                aud_hit=au_heal;
                break;
            }
            case WS_WATER: {
                aud_hit=au_drown;
                break;
            }
            case WS_ELECTRIC: {
                aud_hit=au_elec;
                break;
            }
            case WS_PRESSURE: {
                aud_hit=au_land;
                break;
            }
            default: {
                aud_hit=au_hit;
            }
        }
        switch(weap.getType()) {
            case W_HEAL: {
                sfxeng->playWAV(aud_hit);
                break;
            }
            case W_PRESSURE: {
                setEvent(new EAnim(this,anim_hit,false,0,ESTATE_BUSY,aud_hit));
                break;
            }
            case W_TOUCH: {
                //TODO: play animations (make new Event for this)
                if (direction&(DIR_UP|DIR_DOWN)) sfxeng->playWAV(aud_hit);
                if (direction&DIR_LEFT) setEvent(new ESpeed(this,TSTRIKE,-100,-DSTRIKE,0,ESTATE_BUSY,aud_hit));
                else setEvent(new ESpeed(this,TSTRIKE,-100,DSTRIKE,0,ESTATE_BUSY,aud_hit));
                break;
            }
            case W_WATER: {
                sfxeng->playWAV(aud_hit);
                break;
            }
            case W_STRIKE: case W_EXPLOSION: {
                //TODO: play animations
                sfxeng->playWAV(aud_hit);
                break;
            }
            default: { }
        }
    }
    return newhealth;
}

DeadViking::DeadViking(string imagename, Uint16 xcord, Uint16 ycord, string name):
  Character(imagename,xcord,ycord,name) { }
DeadViking::~DeadViking() { }
