#ifndef _CHARACTERS_COMMON_H_
    #define _CHARACTERS_COMMON_H_
#endif
#ifndef _WEAPONS_H_
    #include "weapons.h"
#endif
#ifndef _OBJECTS_COMMON_H_
    #include "objects_common.h"
#endif

//Falling, Landing
#define T_IRR           2000
#define V_KRIT          1000
#define T_GRAV_EFFECT   10
#define T_AI_EFFECT     20
 
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

class Character : public Object {
    public:
        Character(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Character");
        virtual ~Character();
        Uint8 getHealth();
        Sint16 getSpeed() const {
            return speed;
        }
        Sint16 setSpeed(Sint16 newspeed) {
            return speed=newspeed;
        }
        Sint16 addSpeed(Sint16 dspeed) {
            return speed+=dspeed;
        }
        Sint16 addHSpeed(Sint16 dspeed) {
            return hspeed+=dspeed;
        }
        void setGravity(Sint16 setgravity) {
            gravity=setgravity;
        }
        void addGravity(Sint16 addgravity) {
            gravity+=addgravity;
        }
        void applySpeedMod(Uint16 speedm) {
            speedmod=Uint16(speedmod*speedm/100);
        }
        void setSpeedMod(Uint16 speedm) {
            speedmod=speedm;
        }
        Hit checkMove(SDL_Rect& dest, bool tele=false, bool check=false);
        const std::set<Object *>& getEnter() const {
            return enter;
        }
        const std::set<Object *>& getTouch() const {
            return touch;
        }
        //VIRTUAL METHODS
        virtual void removedObject(Object*);
        //updates the current animation state
        virtual void updateAnimState(bool change=true);
        virtual void resetAnimState() {
            Object::resetAnimState();
            updateAnimState(true);
        }
        //Define these for each character, the base function must be run for all derived classes
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        //when it dies...
        virtual void die();
        //gets hit by a weapon
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
    protected:
        //add places
        virtual void addTouch(std::set<Object *>&);
        virtual void addEnter(std::set<Object *>&);
        //remove places
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        //helper functions
        inline Hit checkHit(const SDL_Rect& dest, Object* destobj, bool tele=false) const;
        inline void updateRegions(std::set<Object *>& newtouch, std::set<Object *>& newenter);
        //health
        Uint8 setHealth(Uint8);
        Uint8 addHealth(Sint8);
        //crash into another object (default: ground)
        virtual void crash(Uint16 dir=DIR_DOWN);
        //move
        virtual Hit move(Uint16 dt, bool check=false);
        Uint8 health;
        Uint8 maxhealth;
        //maximal normal speed;
        Uint16 maxspeedx;
        Uint16 maxspeedy;
        //temporary speed (x direction)
        Sint16 hspeed;
        //current permanent speed (y direction)
        Sint16 speed;
        //current gravity
        Sint16 gravity;
        //current speedmod
        Uint16 speedmod;
        //temporary attributes
        Sint16 Dgrav,Dwater;
        //Die animation
        Animation* im_die;
        //Entered objects
        std::set<Object *> enter;
        //Touched objects
        std::set<Object *> touch;
        //How long can the character swim?
        Uint16 t_water;
        //dense types
        Uint16 dense_types;
        //enemy types
        Uint16 enemy_types;
        //current weapon (updated for each attack)
        Weapon weapon;
    private:
        Uint16 dt_fall2;
};
