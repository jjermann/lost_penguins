#ifndef _CHARACTERS_COMMON_H
#define _CHARACTERS_COMMON_H 1

#include "weapons.h"
#include "objects_common.h"

//Falling, Landing
#define T_IRR           2000
#define V_KRIT          1000
#define T_GRAV_EFFECT   10
#define T_AI_EFFECT     20
#define SPEED_STEP      10
 
//Character states
//facing: either left or right (not left)
#define STATE_LEFT      0x00000001
//in water
#define STATE_WATER     0x00000004
//falling
#define STATE_FALL      0x00000008
//Common Triggers
#define STATE_ACT_1     0x00000010
#define STATE_ACT_2     0x00000020
#define STATE_ACT_3     0x00000040

//Special states
#define STATE_RUN       0x00000080
#define STATE_PRESS_LR  0x00000100
#define STATE_PRESS_UD  0x00000200


/** \brief Damagable, non static objects.

    A character can get hit, can move and fall, can die, etc...
*/
class Character : public Object {
    public:
        Character(string img, Sint16 xpos=0, Sint16 ypos=0, string name="Character");
        virtual ~Character();
        Uint8 getHealth();
        //@{
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
        //@}
        //@{
        void setGravity(Sint16 setgravity) {
            gravity=setgravity;
        }
        void addGravity(Sint16 addgravity) {
            gravity+=addgravity;
        }
        //@}
        //@{
        Uint16 addMaxSpeed(Sint16 dmax) {
            return maxspeedx=max(0,maxspeedx+dmax);
        }
        Uint16 setMaxSpeed(Uint16 maxs) {
            return maxspeedx=maxs;
        }
        //@}
        //@{
        ///\brief Collision detection
        ///
        /// Checks and correspondingly updates a movement and the regions the character is in.
        /// It therefore performs a collision check for all objects in the objectspool and
        /// the map boundary on the left, right and downside. If check is false it updates the
        /// regions accordingly. The destination parameter is changed to be before any dense
        /// obstacles in the way.
        /// \pre The character didn't hit anything
        /// \param dest Designated movement position of the object
        /// \param tele True if it's not a smooth movement but an instant jump, it will
        ///   then check all directions
        /// \param check True if no region updates should be performed
        /// \return Hit type of the movement
        Hit checkMove(SDL_Rect& dest, bool tele=false, bool check=false);
        const std::set<Object *>& getEnter() const {
            return enter;
        }
        const std::set<Object *>& getTouch() const {
            return touch;
        }
        //@}
        //VIRTUAL METHODS
        //@{
        ///\brief Remove the specified object from all internal accountings
        ///
        /// Clean up the internal accountings of the specified object. Usually called
        /// when an object leaves the map (dies).
        virtual void removedObject(Object*);
        ///\brief Updates the current animation
        ///
        /// Checks the state and changes the animation and curpos correpspondingly
        /// \param change False if only the curpos should be changed and not the animation.
        virtual void updateAnimState(bool change=true);
        virtual void resetAnimState() {
            Object::resetAnimState();
            updateAnimState(true);
        }
        //Define these for each character, the base function must be run for all derived classes
        virtual void idle(Uint16);
        /// Calculates the speed of the character
        /// \todo clean up this mess with fall/move
        virtual void fall(Uint16);
        //when it dies...
        virtual void die();
        ///\brief Weapon hits
        ///
        /// Called when hit by a weapon. Depending on the direction and weapon used,
        /// certain effects/events are run.
        /// \param direction Direction from which the weapon deals damage
        /// \param weap Weapon used in the attack/hit
        /// \return New health of the character
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
        //@}
    protected:
        ///\brief Checks the type of the impact into the destination object
        ///
        /// Helper function (run by checkMove()) that checks the type of the
        /// collision into one specific object.
        ///\pre The character didn't hit anything
        ///\param dest Destination rectangle of the movement
        ///\param destobj Target object to check
        ///\param tele If true an instant movement (no smooth movement) is assumed
        ///\return Hit type of the collision, HIT_NOTHING if no valid destobj
        ///   was specified or if the destobj is the character himself.
        inline Hit checkHit(const SDL_Rect& dest, Object* destobj, bool tele=false) const;
        ///\brief Updates the regions
        ///
        /// A helper function (run by checkMove()) that updates the touch and
        /// enter regions of the character. It adds the new regions and removes
        /// the old ones.
        /// \param newtouch Set of all touched objects
        /// \param newenter Set of all entered objects
        inline void updateRegions(std::set<Object *>& newtouch, std::set<Object *>& newenter);
        //@{
        /// Sets the health of the character to a minimum of 0
        /// \return New health
        Uint8 setHealth(Uint8);
        /// Adds the specified amount of life to the health of the character to a minimum of 0
        /// \return New health
        Uint8 addHealth(Sint8);
        //@}
        //crash into another object (default: ground)
        virtual void crash(Uint16 dir=DIR_DOWN);
        /// Updates the position of the character depending on it's velocity, checks for crashes
        /// \todo clean up this mess with move and fall
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
        //temporary attributes
        Sint16 Dgrav;
        //Die animation
        Animation* im_die;
        //Entered objects
        std::set<Object *> enter;
        //Touched objects
        std::set<Object *> touch;
        //dense types
        Uint16 dense_types;
        //enemy types
        Uint16 enemy_types;
        //current weapon (updated for each attack)
        Weapon weapon;
        //@{
        /// Adds the specified set to the touch set
        virtual void addTouch(std::set<Object *>&);
        /// Adds the specified set to the enter set
        virtual void addEnter(std::set<Object *>&);
        /// Removes the specified sets from the touch set
        virtual void removeTouch(std::set<Object *>&);
        /// Removes the specified sets from the enter set
        virtual void removeEnter(std::set<Object *>&);
        //@}
    private:
        Uint16 dt_fall2;
};
#endif
