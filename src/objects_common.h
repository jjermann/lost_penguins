#ifndef _OBJECTS_COMMON_H
#define _OBJECTS_COMMON_H

//Object types
#define OTYPE_DENSE_R   0x00000001
#define OTYPE_DENSE_L   0x00000002
#define OTYPE_DENSE_U   0x00000004
#define OTYPE_DENSE_D   0x00000008
#define OTYPE_DENSE     0x0000000F
#define OTYPE_TOUCH     0x00000020
#define OTYPE_ENTER     0x00000040
#define OTYPE_CHARACTER 0x00000080
#define OTYPE_PLAYER    0x00000100
#define OTYPE_MONSTER   0x00000200
#define OTYPE_ITEM      0x00000400
#define OTYPE_PROJ      0x00000800

//Event states
//busy:  can't do anything interactively
#define ESTATE_BUSY     0x00001000
//abort: can do anything, event is canceled in case of another event
#define ESTATE_ABORT    0x00002000
//run:  default, equivalent to event != NULL
#define ESTATE_RUN      0x00004000
//anim: an animation is beeing played, don't run updateAnimState...
#define ESTATE_ANIM     0x00008000

//OBSOLETE: attack anim (remove this once the attack anim is done)
#define STATE_ATTACK    0x10000000

//Base class of almost everything
class Object {
    public:
        Object(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Object");
        virtual ~Object();
        //uper-left
        bool setPos(Uint16 xcord,Uint16 ycord) {
            if ((xcord<=(maparea->w-pos.w)) && (ycord<=(maparea->h-pos.h))) {
                pos.x=xcord;
                pos.y=ycord;
                return true;
            } else return false;
        }
        //center
        SDL_Rect getCenter() {
            SDL_Rect tmprect;
            tmprect.x=pos.x+Uint16(pos.w/2);
            tmprect.y=pos.y+Uint16(pos.h/2);
            tmprect.w=0;
            tmprect.h=0;
            return tmprect;
        }
        //is the Object in the rectangle rect
        //touch: a touch (partially entered) is enough
        bool isIn(const SDL_Rect& rect, bool touch=false, Uint16 radius=0);
        //uper-left
        SDL_Rect* getPos() {
            return &pos;
        }
        //we want: bottom-middle of pos
        //animation (image) changes (depending on State changes)
        SDL_Rect* getCurPos() {
            return &curpos;
        }
        const Frame& getFrame() const;
        //object type
        Uint16 getType() const {
            return otype;
        }
        void setType(Uint16 newtype) {
            otype|=newtype;
        }
        void unsetType(Uint16 oldtype) {
            otype&=~oldtype;
        }
        void switchType(Uint16 newtype) {
            otype^=newtype;
        }
        string getName() {
            return name;
        }
        //update the actual Animation
        bool updateAnim(Uint16 dt);
        //not to be used with updateAnimState (used eg. with ESTATE_ANIM)
        void setAnim(Animation* anim);
        virtual void resetAnimState();
        bool isRunning() const;
        void mark_delete() {
            delete_flag=true;
        }
        bool isDeleted() {
            return delete_flag;
        }
        //Events (triggered animations/effects)
        void clearEvents();
        void setEvent(Event* ev);
        Uint16 updateEvents(Uint16 dt);
        void cancelEvent();
        void stopEvent();
        //states
        bool getState(Uint32 cstate) const {
            return (state&cstate);
        }
        void setState(Uint32 cstate) {
            state|=cstate;
        }
        void switchState(Uint32 cstate) {
            state^=cstate;
        }
        void unsetState(Uint32 cstate) {
            state&=~cstate;
        }
        //VIRTUAL METHODS
        //When activated by an object
        virtual bool act(Object*) { return false; }
        //When touched by an object
        virtual void touch(Object*) { }
        //When entered by an object
        virtual void enter(Object*) { }
        //When it isn't touched anymore
        virtual void untouch(Object*) { }
        //When an object leaves it
        virtual void leave(Object*) { }
        //permanent effects this base function must be run by all derived classes
        virtual void idle(Uint16) { }
        virtual void destroy() {
            delete this;
        }
    protected:
        Uint16 tbirth;
        Uint32 state;
        Event* event;
        Animation* im_orig;
        Animation* animation;
        //temporary information about where to _draw_ the animation frame:
        //curpos.wh <= pos.wh, curpos.xy = positive dxy
        SDL_Rect curpos;
        SDL_Rect pos;
        //Object type
        Uint16 otype;
        //Object name, can be used as an id
        string name;
        //delete me flag
        bool delete_flag;
};

class Item : public Object {
    public:
        Item(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Item");
        virtual ~Item();
        virtual bool act(Object*) { return false; }
        void setOwner(Player* plr) {
            owner=plr;
        }
    protected:
        Player* owner;
};

class Background : public Object {
    public:
        Background(string imagename);
        virtual ~Background();
};

#endif
