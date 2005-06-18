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
        Uint16 getFPS() {
            return currentfps;
        }
        Uint16 getMinFPS() {
            return minfps;
        }
    private:
        Uint16 tstart, tcurrent, dt;
        bool reset_time;
        Uint16 Dfps,Dframes,currentfps,minfps;
    private:
        inline void updateGame();
        inline void updateEdit();
        inline void updatePaused();
        inline void updateFPS();
};

#endif
