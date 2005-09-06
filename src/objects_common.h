#ifndef DEF_OBJECTS_COMMON_H
#define DEF_OBJECTS_COMMON_H 1

//Object types
//dense types are the same as directions (that fact is beeing used)
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

/** \brief Base class of all objects on the map.

    An object has a graphical representation (animation), a position, an object type,
    a type and can be entered/left/touched/untouched/activated.
*/
class Object {
    public:
        Object(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Object");
        virtual ~Object();
        /// Sets the new position. If the coordinates are not in the map area
        /// they are corrected to fit into it.
        /// \return True if no correction was needed
        bool setPos(Sint16 xcord,Sint16 ycord);
        /// \brief Returns the center of an object
        /// \return Center position of the object with width=height=0
        SDL_Rect getCenter() {
            SDL_Rect tmprect;
            tmprect.x=pos.x+Uint16(pos.w/2);
            tmprect.y=pos.y+Uint16(pos.h/2);
            tmprect.w=0;
            tmprect.h=0;
            return tmprect;
        }
        /// Checks whether this object is partially inside a certain map area.
        /// \param rect Map area
        /// \param touch If true an overlapping region is enough, if false
        ///        the object center has to be inside the specified area.
        /// \return True if the object is inside
        bool isIn(const SDL_Rect& rect, bool touch=false);
        SDL_Rect* getPos() {
            return &pos;
        }
        SDL_Rect getDrawPos() const;
        const Frame getFrame() const;
        //@{
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
        //@}
        string getName() {
            return name;
        }
        //@{
        /// Updates the current animation
        /// \return True if the animation is still running
        bool updateAnim(Uint16 dt);
        /// Specifies the animation to be run (usually a one time animation)
        /// and starts it right away. Should not be used with updateAnimState,
        /// set ESTATE_ANIM to deactivate an updateAnimState. This is usually
        /// controlled by an AnimationEvent
        bool setAnim(EmptyAnimationPtr anim, bool start=false);
        /// Sets the animation back to the default one.
        virtual void resetAnimState();
        bool isRunning() const;
        //@}
        //@{
        /// Mark an object for deletion.
        void mark_delete() {
            delete_flag=true;
        }
        bool isDeleted() {
            return delete_flag;
        }
        virtual void destroy() {
            delete this;
        }
        //@}
        /// Load an animation bound onto this object from an animation data file
        EmptyAnimationPtr loadAnimation(string anim_name,
                                        double scale_factor=1,
                                        BasePointType abp_type=BP_MD,
                                        Uint16 aanimation_type=ATYPE_LOOP,
                                        double afps=0,
                                        AllignType aallign_type=AT_MD);
        /// Load an animation bound onto this object
        EmptyAnimationPtr loadAnimation(const Image& abase_image,
                                        Uint16 aframes=1,
                                        BasePointType abp_type=BP_MD,
                                        Uint16 aanimation_type=ATYPE_LOOP,
                                        double afps=0,
                                        Uint16 astart_pos=0,
                                        AllignType aallign_type=AT_MD);
        //Events (triggered animations/effects)
        //@{
        /// Clears the event field (sets it to NULL). This should only be used by
        /// the event destructor.
        /// \todo Is there a way to solve this better?
        void clearEvents();
        /// Schedules a new event and cancels any currently running event
        /// \param ev New event
        virtual void setEvent(Event* ev);
        /// Updates the event (run by the PhysicHandler) and react on the new event
        /// state (start, end or cancel an event).
        /// \return New event state
        Uint16 updateEvents(Uint16 dt);
        /// Cancel the current event
        void cancelEvent();
        /// End the current event
        void stopEvent();
        //@}
        //states
        //@{
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
        //@}
        //VIRTUAL METHODS
        //@{
        /// Try to activate any objects in the current position
        /// \return True if an object could be activated
        virtual bool act(Object*) { return false; }
        /// Touch an object, called when the object position overlaps with the position
        /// of another object for the first time.
        virtual void touch(Object*) { }
        /// Enter an object, called when the object center enters another object
        /// position for the first time.
        virtual void enter(Object*) { }
        /// Untouch a touched object, called when the object no longer overlaps with
        /// another object.
        virtual void untouch(Object*) { }
        /// Leave an object, called when the object center no longer is inside another
        /// object.
        virtual void leave(Object*) { }
        /// What should the object do all the time? This is run first of all.
        /// \remark This should be called downwards by all derived classes
        virtual void idle(Uint16) { }
        //@}
        bool operator<(const Object& obj) const;
        Uint32 onum;
    protected:
        Uint32 state;
        Event* event;
        ///\todo Document this!
        /// Upper left logical position of the object (used for decisions)
        SDL_Rect pos;
        //Object type
        Uint16 otype;
        //Object name, can be used as an id
        string name;
        //delete me flag
        bool delete_flag;
        EmptyAnimationPtr anim_orig;
        EmptyAnimationPtr animation;
};

/** \brief Item

    An item doesn't interact with other objects besides players who can pick them
    up and store, use or drop them
*/
class Item : public Object {
    public:
        Item(string img, Sint16 xpos=0, Sint16 ypos=0, string name="Item");
        virtual ~Item();
        virtual bool act(Object*) { return false; }
        /// Assigns the item a new (player) owner
        void setOwner(Player* plr) {
            owner=plr;
        }
    protected:
        Player* owner;
};

/** \brief Background

*/
class Background : public Object {
    public:
        Background(string imagename);
        virtual ~Background();
};

#endif
