#ifndef _PLAYERS_COMMON_H
#define _PLAYERS_COMMON_H 1

#include "characters_common.h"

#define MAX_ITEMS       3
   
//Player states
//movement (while up/down/left/right key is pressed)
#define STATE_MLEFT     0x00000004
#define STATE_MRIGHT    0x00000008
#define STATE_MUP       0x00000010
#define STATE_MDOWN     0x00000020

/** \brief Interactively controlled characters

*/
class Player : public Character {
    public:
        Player(string img, Sint16 xpos=0, Sint16 ypos=0, string name="Player");
        virtual ~Player();
        //Items
        //returns icon for the player bar
        Frame getIcon() const;
        Uint8 getItemNum() {
            return currentitem;
        }
        Item* getItem(Uint8 num) {
            if (num<MAX_ITEMS) return items[num];
            else return NULL;
        }
        bool pickupItem(Item* newitem);
        Item* moveItem() {
            return moveItem(currentitem);
        }
        void removeItem() {
            removeItem(currentitem);
        }
        void switchItem(bool right=true);
        Object* dropItem() {
            return dropItem(currentitem);
        }
        //VIRTUAL METHODS
        //updates the current animation state
        virtual void updateAnimState(bool change=true);
        //Define these for each player...
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        virtual void in_right(Sint16);
        virtual void in_left(Sint16);
        virtual void in_up(Sint16);
        virtual void in_down(Sint16);
        virtual void in_sp1(Sint16);
        virtual void in_sp2(Sint16);
        //This should be the same for all players
        virtual void in_act(Sint16);
        virtual void in_use(Sint16);
        //gets hit by a weapon
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
        //removes all interactively initiated events/states, optionally reset states/etc...
        virtual void clearStates(bool reset=false);
    protected:
        //add places
        virtual void addTouch(std::set<Object *>&);
        virtual void addEnter(std::set<Object *>&);
        //remove places
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        //Items
        void removeItem(Uint8 num);
        Item* moveItem(Uint8 num);
        Object* dropItem(Uint8 num);
        //crash into another object (default: ground)  
        virtual void crash(Uint16 dir=DIR_DOWN);
        //when it dies...
        virtual void die();
        //move
        virtual Hit move(Uint16 dt, bool check=false);
        //Common animations
        Animation* im_left;
        Animation* im_right;
        Animation* im_fall_left;
        Animation* im_fall_right;
        Animation* im_krit_left;
        Animation* im_krit_right;
        Animation* im_land_left;
        Animation* im_land_right;
        //Common audio chunks
        Mix_Chunk*  au_hit;
        Mix_Chunk*  au_land;
        Mix_Chunk*  au_act;
        Mix_Chunk*  au_newitem;
        Mix_Chunk*  au_die;
        Mix_Chunk*  au_elec;
        Mix_Chunk*  au_drown;
        Mix_Chunk*  au_fire;
        Mix_Chunk*  au_heal;
        //Items
        Item* items[MAX_ITEMS];
        Uint8 currentitem;
};

class DeadPlayer : public Character {
    public:
        DeadPlayer(string imagename, Sint16 xcord=0, Sint16 ycord=0, string name="DeadPlayer");
        virtual ~DeadPlayer();
};

#endif
