#ifndef _VIKINGS_COMMON_H_
    #define _VIKINGS_COMMON_H_
#endif
#ifndef _CHARACTERS_COMMON_H_
    #include "characters_common.h"
#endif

#define MAX_ITEMS       3
   
//Character/Viking states
//facing: either left or right (not left)
#define STATE_LEFT      0x00000001
//movement (while up/down/left/right key is pressed)
#define STATE_MLEFT     0x00000004
#define STATE_MRIGHT    0x00000008
#define STATE_MUP       0x00000010
#define STATE_MDOWN     0x00000020
//in water
#define STATE_WATER     0x00000040
//falling (normal and fast)
#define STATE_FALL      0x00000080
#define STATE_FALL2     0x00000100
//Common Triggers
#define STATE_ACT_1     0x00000200
#define STATE_ACT_2     0x00000400
#define STATE_ACT_3     0x00000800

//TODO: somehow differentiate between characters, vikings and AI players or objects
class Viking : public Character {
    public:
        Viking(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Viking");
        virtual ~Viking();
        //Items
        //returns icon for the viking bar
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
        Object* dropItem(bool right=true) {
            return dropItem(currentitem,right);
        }
        //VIRTUAL METHODS
        //updates the current animation state
        virtual void updateAnimState(bool change=true);
        //Define these for each viking...
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        virtual void in_right(Sint16);
        virtual void in_left(Sint16);
        virtual void in_up(Sint16);
        virtual void in_down(Sint16);
        virtual void in_sp1(Sint16);
        virtual void in_sp2(Sint16);
        //This should be the same for all vikings
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
        Object* dropItem(Uint8 num, bool right=true);
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
        Mix_Chunk*  au_land;
        Mix_Chunk*  au_act;
        Mix_Chunk*  au_newitem;
        Mix_Chunk*  au_hit;
        Mix_Chunk*  au_die;
        Mix_Chunk*  au_elec;
        Mix_Chunk*  au_drown;
        Mix_Chunk*  au_fire;
        Mix_Chunk*  au_heal;
        //Items
        Item* items[MAX_ITEMS];
        Uint8 currentitem;
};

class DeadViking : public Character {
    public:
        DeadViking(string imagename, Uint16 xcord=0, Uint16 ycord=0, string name="DeadViking");
        virtual ~DeadViking();
};
