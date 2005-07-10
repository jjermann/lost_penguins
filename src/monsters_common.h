#ifndef DEF_MONSTERS_COMMON_H
#define DEF_MONSTERS_COMMON_H 1

#include "characters_common.h"

/** \brief Intelligent opponents

    \todo Clean up the mess of AI functions.
*/
class Monster : public Character {
    public:
        Monster(string img, Sint16 xpos=0, Sint16 ypos=0, string name="Monster");
        virtual ~Monster();
        virtual void removedObject(Object*);
        virtual void updateAnimState(bool change=true);
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
    protected:
        /// Runs Character::addTouch() and updates the targets
        /// \todo Clean up this mess
        virtual void addTouch(std::set<Object *>&);
        virtual void addEnter(std::set<Object *>&);
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        virtual void crash(Uint16 dir=DIR_DOWN);
        virtual void die();
        //ai functions
        //@{
        virtual void runAI(Uint16) { }
        virtual void ai_left(Uint16) { }
        virtual void ai_right(Uint16) { }
        virtual void ai_attack(Player*, Uint16) { }
        //@}
        //Attack targets
        std::set<Player *> targets;
        virtual Hit move(Uint16 dt, bool check=false);
        //common animations
        Animation* im_left;
        Animation* im_right;
        Mix_Chunk* au_hit;
        Sint16 Dai,Dattack;
};

#endif
