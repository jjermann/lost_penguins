#ifndef _MONSTERS_COMMON_H
#define _MONSTERS_COMMON_H

#include "characters_common.h"

//Monsters
class Monster : public Character {
    public:
        Monster(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Monster");
        virtual ~Monster();
        virtual void removedObject(Object*);
        //updates the current animation state
        virtual void updateAnimState(bool change=true);
        //Define these for each player...
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        //gets hit by a weapon
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
        //TODO: add AI commands?
    protected:
        //add places
        virtual void addTouch(std::set<Object *>&);
        virtual void addEnter(std::set<Object *>&);
        //remove places
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        //crash into another object (default: ground)  
        virtual void crash(Uint16 dir=DIR_DOWN);
        //when it dies...
        virtual void die();
        //ai functions
        virtual void runAI(Uint16) { }
        virtual void ai_left(Uint16) { }
        virtual void ai_right(Uint16) { }
        virtual void ai_attack(Player*, Uint16) { }
        //Attack targets
        std::set<Player *> targets;
        //move
        virtual Hit move(Uint16 dt, bool check=false);
        //common animations
        Animation* im_left;
        Animation* im_right;
        Sint16 Dai,Dattack;
};

#endif
