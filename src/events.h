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
        /// \param obj Object that controls the event
        /// \param length Time after start until the event ends
        /// \param edelay Time until the event starts
        /// \param switchstate State changes between start and end/cancel
        Event(Object* obj, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING);
        /// \brief Resets the event state of owner
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
        /// \param asound Sound (Mix_Chunk) to be played when the event starts
        /// \param runanim Animation to be run when the event starts
        /// \todo Get rid of the delanim parameter
        /// \param delanim True if the animation should be deleted
        AnimEvent(Object* obj, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING,
          Mix_Chunk* asound=NULL, Animation* runanim=NULL, bool delanim=false);
        /// \brief Updates the events
        /// \return Event state: either delayed (EV_DELAY), starting (EV_START),
        ///         running (EV_RUN), stopping (EV_END) or canceled (EV_CANCEL)
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
        CAnimEvent(Character* chr, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING,
          Mix_Chunk* asound=NULL, Animation* runanim=NULL, bool delanim=false);
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
    target types to (try to) hit.
*/
class EAttack : public CAnimEvent {
    public:
        /// \param atweapon Weapon to be used for the attack
        /// \param dir Directions to attack
        /// \param weapon_range Range of the weapon starting from the
        ///        character into all specified directions
        /// \param target_mask Mask for the object type of the targets to be hit
        EAttack(Character* chr, Uint16 length, Weapon* atweapon, Uint16 dir, Uint16 weapon_range=0,
          Uint16 target_mask=NOTHING, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL,
          Animation* runanim=NULL, bool delanim=false);
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
        /// \param avspeed Vertical speed to be add when the event starts
        /// \param ahspeed Horizontal speed to be add when the event starts
        ESpeed(Character* chr, Uint16 length, Sint16 avspeed, Sint16 ahspeed=0, Uint16 edelay=0,
          Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
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
        /// Adds the initial speed
        /// \param inispeed Initial horizontal speed to be added
        /// \param ahspeed Horizontal speed to be added when the event starts
        ERun(Character* chr, Uint16 length, Sint16 inispeed, Sint16 ahspeed, Uint16 edelay=0,
          Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual ~ERun();
        /// Forces the event to continue
        virtual void reset(); 
        /// If the event wasn't forced to continue it will shortly end
        virtual Uint16 update(Uint16 dt);
    protected:
        Sint16 ispeed;
        Sint16 t_reset;
};

#endif
