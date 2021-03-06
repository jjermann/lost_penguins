#ifndef DEF_EVENTS_H
#define DEF_EVENTS_H 1

#define EV_DELAY     0x00000001
#define EV_START     0x00000002
#define EV_RUN       0x00000004
#define EV_END       0x00000008
#define EV_CANCEL    0x00000010

/** \brief Base class for events (non instantanious effects)

    Organize effects that aren't instantanious. Updated by PhysicHandler, usually
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
        /// \param obj Owner object of the AnimEvent
        /// \param length Duration of the animation, once it's started
        /// \param edelay Delay until the animation is started
        /// \param switchstate States that will be switched when the animation starts/ends
        /// \param asound Sound (Mix_Chunk) to be played when the event starts
        /// \param runanim Animation to be run when the event starts
        /// \todo Get rid of the delanim parameter
        AnimEvent(Object* obj, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING,
          Mix_Chunk* asound=NULL, EmptyAnimationPtr runanim=EmptyAnimationPtr());
        /// \brief Updates the events
        /// \return Event state: either delayed (EV_DELAY), starting (EV_START),
        ///         running (EV_RUN), stopping (EV_END) or canceled (EV_CANCEL)
        virtual Uint16 update(Uint16 dt);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        EmptyAnimationPtr anim;
        Mix_Chunk* sound;
};
  
/** \brief Character animation event

    Same as Animation event for characters.
*/
class CAnimEvent : public CEvent {
    public:
        CAnimEvent(Character* chr, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING,
          Mix_Chunk* asound=NULL, EmptyAnimationPtr runanim=EmptyAnimationPtr());
        virtual Uint16 update(Uint16 dt);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        EmptyAnimationPtr anim;
        Mix_Chunk* sound;
};

/** \brief Attack event

    Describes a full attack including weapons used, direction of attack, weapon range,
    target types to (try to) hit.
*/
class EAttack : public CAnimEvent {
    public:
        /// \param chr Character owner of the AnimEvent
        /// \param length Duration of the animation, once it's started
        /// \param atweapon Weapon to be used for the attack
        /// \param dir Directions to attack
        /// \param weapon_range Range of the weapon starting from the
        ///        character into all specified directions
        /// \param target_mask Mask for the object type of the targets to be hit
        /// \param edelay Delay until the animation is started
        /// \param switchstate States that will be switched when the animation starts/ends
        /// \param esound Sound (Mix_Chunk) to be played when the event starts
        /// \param runanim Animation to be run when the event starts
        EAttack(Character* chr, Uint16 length, Weapon* atweapon, Uint16 dir, Uint16 weapon_range=0,
          Uint16 target_mask=NOTHING, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL,
          EmptyAnimationPtr runanim=EmptyAnimationPtr());
        virtual void end();
    protected:
        Weapon* weapon;
        Uint16 direction;
        Uint16 range;
        Uint16 mask;
};

/** \brief Delayed (accelerated) run event

    A special speed event that increases the horizontal speed after a while,
    has to be constantly maintained using reset().
*/
class ERun : public CAnimEvent {
    public:
        /// Adds the initial speed
        ERun(Character* chr, Uint16 length, Sint16 edmax, Uint16 edelay=0,
          Uint32 switchstate=0, Mix_Chunk* esound=NULL, EmptyAnimationPtr runanim=EmptyAnimationPtr());
        virtual ~ERun();
        /// Forces the event to continue
        virtual void start(); 
        virtual void end(); 
        virtual void cancel(); 
        virtual void reset(); 
        /// If the event wasn't forced to continue it will shortly end
        virtual Uint16 update(Uint16 dt);
    protected:
        Sint16 dmax;
        Sint16 t_reset;
};

#endif
