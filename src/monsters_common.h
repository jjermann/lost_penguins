#ifndef _MONSTERS_COMMON_H_
    #define _MONSTERS_COMMON_H_
#endif
#ifndef _CHARACTERS_COMMON_H_
    #include "characters_common.h"
#endif

//Monsters
class Monster : public Character {
    public:
        Monster(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Monster");
        virtual ~Monster();
        virtual void removedObject(Object*);
        //updates the current animation state
        virtual void updateAnimState(bool change=true);
        //Define these for each viking...
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
        virtual void ai_attack(Viking*, Uint16) { }
        Sint16 Dai,Dattack;
        //Attack targets
        std::set<Viking *> targets;
        //move
        virtual Hit move(Uint16 dt, bool check=false);
        //common animations
        Animation* im_left;
        Animation* im_right;
};