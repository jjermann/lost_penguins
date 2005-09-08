#include "common.h"
#include "events.h"
#include "input.h"
#include "animation.h"
#include "weapons.h"
#include "sfxeng.h"
#include "imgcache.h"
#include "sndcache.h"
#include "objectpools.h"
#include "scenario.h"
//to allow gfxeng->update()
#include "gfxeng.h"
#include "players_common.h"


Player::Player(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Character(xcord,ycord,parameters),
  currentitem(0),
  anim_left(new EmptyAnimation(&anim_right)),
  anim_right(new EmptyAnimation()),
  anim_rock_left(new EmptyAnimation(&anim_left)),
  anim_rock_right(new EmptyAnimation(&anim_right)),
  anim_walk_left(new EmptyAnimation(&anim_left)),
  anim_walk_right(new EmptyAnimation(&anim_right)),
  anim_push_left(new EmptyAnimation(&anim_left)),
  anim_push_right(new EmptyAnimation(&anim_right)),
  anim_fall_left(new EmptyAnimation(&anim_left)),
  anim_fall_right(new EmptyAnimation(&anim_right)),
  anim_fall_fast_left(new EmptyAnimation(&anim_fall_left)),
  anim_fall_fast_right(new EmptyAnimation(&anim_fall_right)),
  anim_crash_left(new EmptyAnimation()),
  anim_crash_right(new EmptyAnimation()),
  anim_rope_left(new EmptyAnimation(&anim_left)),
  anim_rope_right(new EmptyAnimation(&anim_right)),
  anim_teleport_left(new EmptyAnimation()),
  anim_teleport_right(new EmptyAnimation()),
  anim_die_crash_left(new EmptyAnimation(&anim_die_bones_left)),
  anim_die_crash_right(new EmptyAnimation(&anim_die_bones_right)),
  anim_die_burn_left(new EmptyAnimation(&anim_die_bones_left)),
  anim_die_burn_right(new EmptyAnimation(&anim_die_bones_right)),
  anim_die_bones_left(new EmptyAnimation(&anim_die_bones_right)),
  anim_die_bones_right(new EmptyAnimation()),
  anim_die_elec_left(new EmptyAnimation(&anim_die_bones_left)),
  anim_die_elec_right(new EmptyAnimation(&anim_die_bones_right)),
  anim_die_spike_left(new EmptyAnimation(&anim_die_bones_left)),
  anim_die_spike_right(new EmptyAnimation(&anim_die_bones_right)),
  anim_die_water_left(new EmptyAnimation(&anim_die_bones_left)),
  anim_die_water_right(new EmptyAnimation(&anim_die_bones_right)),
  anim_fall_middle(new EmptyAnimation(&anim_fall_right)),
  anim_climb(new EmptyAnimation(&anim_right)),
  anim_bar(new EmptyAnimation(&anim_right)) {
    for (Uint8 i=0; i<MAX_ITEMS; i++) {
        items[i]=NULL;
    }
    state=STATE_FALL;
    otype|=OTYPE_PLAYER;
    enemy_types|=OTYPE_MONSTER;
    au_land=scenario->sndcache->loadWAV("dizzy.wav");
    au_act=scenario->sndcache->loadWAV("button.wav");
    au_useerror=scenario->sndcache->loadWAV("useerror.wav");
    au_newitem=scenario->sndcache->loadWAV("pickup.wav");
    au_hit=scenario->sndcache->loadWAV("vikhit.wav");
    au_elec=scenario->sndcache->loadWAV("elecdth.wav");
    au_drown=scenario->sndcache->loadWAV("drown.wav");
    au_fire=scenario->sndcache->loadWAV("fireball.wav");
    au_die=scenario->sndcache->loadWAV("bones.wav");
    au_heal=scenario->sndcache->loadWAV("usefood1.wav");

    /* Parameters */
    if (!hasParam(parameters,"maxhealth")) maxhealth=4;
    if (!hasParam(parameters,"health")) health=min(3,(int)maxhealth);
    if (!hasParam(parameters,"maxspeedx")) maxspeedx=300;
    if (!hasParam(parameters,"maxspeedy")) maxspeedy=200;
}

Player::~Player() {
    for (Uint8 i=0; i<MAX_ITEMS; i++) {
        if (items[i]) {
            removeItem(i);
        }
    }
}

Frame Player::getIcon() const {
    if (anim_bar) return (anim_bar->getBaseFrame());
    else if (anim_right) return (anim_right->getBaseFrame());
    else return (anim_orig->getBaseFrame());
}

void Player::addEnter(std::set<Object *>& aset) {
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

void Player::addTouch(std::set<Object *>& aset) {
    Character::addTouch(aset);
}
void Player::removeEnter(std::set<Object *>& rset) {
    Character::removeEnter(rset);
}
void Player::removeTouch(std::set<Object *>& rset) {
    Character::removeTouch(rset);
}

bool Player::pickupItem(Item* newitem) {
    if (newitem==NULL) return false;
    for (Uint8 i=0; i<MAX_ITEMS; i++) {
        if (items[i] == NULL) {
            sfxeng->playWAV(au_newitem);
            newitem->setOwner(this);
            newitem->setPos(0,0);
            scenario->pool->moveObject(newitem);
            items[i]=newitem;
            return true;
        }
    }
    return false;
}

void Player::removeItem(Uint8 num) {
    gfxeng->update(UPDATE_BAR);
    if (num>=MAX_ITEMS || items[num]==NULL) return;
    delete items[num];
    items[num]=NULL;
}

Item* Player::moveItem(Uint8 num) {
    gfxeng->update(UPDATE_BAR);
    if (num>=MAX_ITEMS || items[num]==NULL) return NULL;
    Item* tmpit=items[num];
    items[num]=NULL;
    return tmpit;
}

void Player::switchItem(bool right) {
    gfxeng->update(UPDATE_BAR);
    if (right) {
        ++currentitem;
        if (currentitem>=MAX_ITEMS) currentitem=0;
    } else {
        if (currentitem>0) --currentitem;
        else currentitem=(MAX_ITEMS-1);
    }
}

Object* Player::dropItem(Uint8 num) {
    gfxeng->update(UPDATE_ALL);
    Item* tmpit=moveItem(num);
    Object* tmpobj=NULL;
    bool ok=false;
    if (tmpit == NULL) return NULL;
    //check right/left according to (bool right)
    if (state&STATE_LEFT) ok=tmpit->setPos(pos.x - tmpit->getPos()->w - 1, Uint16(getCenter().y-tmpit->getPos()->h/2));
    else ok=tmpit->setPos(pos.x + pos.w + 1, Uint16(getCenter().y-tmpit->getPos()->h/2));
    //new position in map and valid
    if (ok && ((tmpobj=scenario->pool->addObject(tmpit))!=NULL)) return tmpobj;
    else { 
        pickupItem(tmpit);
        return NULL;
    }
}

void Player::setEvent(Event* ev) {
    clearStates();
    Character::setEvent(ev);
}

void Player::updateAnimState() {
    if (state&STATE_LEFT) {
        if (state&STATE_FALL) {
            if (speed>V_KRIT) {
                setAnim(anim_fall_fast_left);
            } else if (state&STATE_MLEFT || speed < 0) {
                setAnim(anim_fall_left);
            } else {
                anim_fall_middle->setFallBack(&anim_fall_left);
                setAnim(anim_fall_middle);
            }
        } else if (state&STATE_MLEFT) {
            setAnim(anim_walk_left);
        } else {
            setAnim(anim_left);
        }
    } else {
        if (state&STATE_FALL) {
            if (speed>V_KRIT) {
                setAnim(anim_fall_fast_right);
            } else if (state&STATE_MRIGHT || speed < 0) {
                setAnim(anim_fall_right);
            } else {
                anim_fall_middle->setFallBack(&anim_fall_right);
                setAnim(anim_fall_middle);
            }
        } else if (state&STATE_MRIGHT) {
            setAnim(anim_walk_right);
        } else {
            setAnim(anim_right);
        }
    }
}

void Player::idle(Uint16 dt) {
    Character::idle(dt);
}

void Player::in_act() {
    Mix_Chunk* au_act_bak=au_act;
    object_iterator i=enter.begin();
    while (i!=enter.end()) {
        if ((*i)->act(this)) {
            sfxeng->playWAV(au_act_bak);
            return;
        }
        ++i;
    }
}

void Player::in_use() {
    if (items[currentitem]) {
        if (!(items[currentitem]->act(this))) sfxeng->playWAV(au_useerror);
    }
}

void Player::in_right(Uint16 dt) {
    unsetState(STATE_LEFT);
    if ((hspeed+HSPEED_MULT*dt/100)<maxspeedx) hspeed+=HSPEED_MULT*dt/100;
    else if (hspeed<maxspeedx) hspeed=maxspeedx;
}

void Player::in_left(Uint16 dt) {
    setState(STATE_LEFT);
    if ((hspeed-HSPEED_MULT*dt/100)>(-maxspeedx)) hspeed-=HSPEED_MULT*dt/100;
    else if (hspeed>(-maxspeedx)) hspeed=-maxspeedx;
}

Hit Player::move(Uint16 dt, bool check) {
    return Character::move(dt,check);
}

void Player::fall(Uint16 dt) {
    if (!getState(STATE_MRIGHT|STATE_MLEFT)) {
        if (!getState(STATE_FALL)) hspeed=addAbsolute(hspeed,-dt*HSPEED_MULT/100);
        else hspeed=addAbsolute(hspeed,-dt*HSPEED_MULT/200);
    }
    Character::fall(dt);
}

void Player::crash(Uint16 dir) {
    if (event) event->cancel();
    switch (dir) {
        case DIR_LEFT: {
            hspeed=0;
            break;
        }
        case DIR_RIGHT: {
            hspeed=0;
            break;
        }
        case DIR_UP: {
            speed=0;
            break;
        }
        //if unsure, land on ground...
        case DIR_DOWN: default : {
            clearStates(true);
            unsetState(STATE_FALL);
            Dgrav=0;
            if (speed>V_KRIT) {
                speed=0;
                Weapon tmpweap(-1,W_PRESSURE,WS_PRESSURE);
                hit(DIR_UP,tmpweap);
            } else {
                speed=0;
            }
            break;
        }
    }
}

void Player::die() {
    /* todo add box and ask for a continue / redo */
    scenario->failed=true;
    Character::die();
}

void Player::clearStates(bool reset) {
    if (reset) {
        unsetState(STATE_ACT_1);
        unsetState(STATE_ACT_2);
        unsetState(STATE_ACT_3);
    }
    unsetState(STATE_MLEFT);
    unsetState(STATE_MRIGHT);
}

Uint16 Player::hit(Uint16 direction, Weapon& weap) {
    cancelEvent();
    Uint16 newhealth;
    if (weap.getType()&W_WATER) newhealth=setHealth(0);
    else newhealth=addHealth(weap.getDamage());
    
    //died
    if (newhealth==0) {
        die();
        //should be !=NULL or sthg is wrong with the placement code...
        ParameterMap deadplr_parameters;
        deadplr_parameters["image"]="dead_player.bmp";
        Character* deadplr=scenario->pool->addCharacter(new DeadPlayer(pos.x,pos.y,deadplr_parameters));
        switch(weap.getSubType()) {
            case WS_FIRE: {
                if (deadplr) deadplr->setEvent(new CAnimEvent(deadplr,10,0,ESTATE_BUSY,au_fire,(state&STATE_LEFT) ? anim_die_burn_left : anim_die_burn_right));
                else sfxeng->playWAV(au_fire);
                break;
            }
            case WS_WATER: {
                if (deadplr) deadplr->setEvent(new CAnimEvent(deadplr,10,0,ESTATE_BUSY,au_drown,(state&STATE_LEFT) ? anim_die_water_left : anim_die_water_right));
                else sfxeng->playWAV(au_drown);
                break;
            }
            //WS_NORMAL, WS_ELECTRIC, WS_FIRE, WS_PRESSURE
            default: {
                if (deadplr) deadplr->setEvent(new CAnimEvent(deadplr,10,0,ESTATE_BUSY,au_die,(state&STATE_LEFT) ? anim_die_bones_left : anim_die_bones_right));
                else sfxeng->playWAV(au_die);
            }
        }
    //damaged or healed...
    } else {
        Mix_Chunk* aud_hit;
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
                setEvent(new CAnimEvent(this,T_IRR,0,ESTATE_BUSY,aud_hit,(state & STATE_LEFT) ? anim_crash_left : anim_crash_right));
                break;
            }
            case W_TOUCH: {
                //TODO: play animations (make new Event for this)
                if (direction&(DIR_UP|DIR_DOWN)) sfxeng->playWAV(aud_hit);
                if (direction&DIR_LEFT) {
                    addSpeed(-200);
                    addHSpeed(-400);
                    setEvent(new CAnimEvent(this,TSTRIKE,0,ESTATE_BUSY,aud_hit));
                } else {
                    addSpeed(-200);
                    addHSpeed(400);
                    setEvent(new CAnimEvent(this,TSTRIKE,0,ESTATE_BUSY,aud_hit));
                }
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

DeadPlayer::DeadPlayer(Sint16 xcord, Sint16 ycord, ParameterMap& parameters):
  Character(xcord,ycord,parameters) {
}
DeadPlayer::~DeadPlayer() { }
