#ifndef _EVENTS_H
#define _EVENTS_H 1

#define EV_DELAY     0x00000001
#define EV_START     0x00000002
#define EV_RUN       0x00000004
#define EV_END       0x00000008
#define EV_CANCEL    0x00000010

/// \todo: This is already defined in eric's header, either remove it there or find
///        a more intelligent way to deal with it.
#define STATE_RUN        0x00400000

/** \brief Base class for events (non instantanious effects)

    Organize effects that aren't instantanious. Updated by AnimHandler, usually
    a member of an Object (controlled by the object). An effect can be delayed and has a
    certain run period (marked by a state).
*/
class Event {
    public:
        Event(Object* obj, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING);
        virtual ~Event();
        virtual Uint16 update(Uint16 dt);   
        virtual void cancel();
        virtual void start();
        virtual void end();
        virtual void reset() { }
    protected:
        Object* owner;
        Uint16 duration;
        Uint16 delay;
        Sint16 tleft;
        bool started;
        Uint32 state;
};

/** \brief Character event

    Same as Event for characters.
*/
class CEvent : public Event {
    public:
        CEvent(Character* chr, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING);
    protected:
        Character* charowner;
};

/** \brief Animation event

    An animated effect that usually ends when the animation is finnished.
*/
class AnimEvent : public Event {
    public:
        AnimEvent(Object* obj, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING, Mix_Chunk* asound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual Uint16 update(Uint16 dt);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        Animation* anim;
        bool del;
        Mix_Chunk* sound;
};
  
/** \brief Character animation event

    Same as Animation event for characters.
*/
class CAnimEvent : public CEvent {
    public:
        CAnimEvent(Character* chr, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING, Mix_Chunk* asound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual Uint16 update(Uint16 dt);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        Animation* anim;
        bool del;
        Mix_Chunk* sound;
};

/** \brief Attack event

    Describes a full attack including weapons used, direction of attack, weapon range,
    targets to (try to) hit.
*/
class EAttack : public CAnimEvent {
    public:
        EAttack(Character* chr, Uint16 length, Weapon* atweapon, Uint16 dir, Uint16 weapon_range=0, Uint16 target_mask=NOTHING, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual void end();
    protected:
        Weapon* weapon;
        Uint16 direction;
        Uint16 range;
        Uint16 mask;
};

/** \brief Speed modification event

    Modifies the vertical and/or the horizontal speed.
*/
class ESpeed : public CAnimEvent {
    public:
        ESpeed(Character* chr, Uint16 length, Sint16 avspeed, Sint16 ahspeed=0, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        Sint16 vspeed;
        Sint16 hspeed;
};

/** \brief Delayed (accelerated) run event

    A special speed event that increases the horizontal speed after a while,
    has to be constantly maintained using reset().
*/
class ERun : public ESpeed {
    public:
        ERun(Character* chr, Uint16 length, Sint16 inispeed, Sint16 ahspeed, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual ~ERun();
        virtual void reset(); 
        virtual Uint16 update(Uint16 dt);
    protected:
        Sint16 ispeed;
        Sint16 t_reset;
};

#endif
