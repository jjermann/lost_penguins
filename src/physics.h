#ifndef _PHYSICS_H  
#define _PHYSICS_H 1

/** \brief Updates all game states and animations, acts as a Mediator between objects

*/
class PhysicHandler  {
    public:
        PhysicHandler();
        ~PhysicHandler();
        /// Updates all game states and animations
        void update();
        /// Resets the current time
        /// \return New current time
        Uint16 resetTime();
    private:
        Uint16 tstart, tcurrent, dt;
};

#endif
