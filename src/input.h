#ifndef _INPUT_H
#define _INPUT_H 1

/** \brief Handels keyboard events

    \remark A key release event which is passed on to the PhysicHandler
      mustn't be cummulative (see player code as well). Ie. If the event
      is triggered twice it should do the same as if once... 
*/
class InputHandler {
    public:
        InputHandler();
        ~InputHandler();
        /// Update the input status
        void update();
        bool keyPressed(ConfigKey key);
        bool keyState(ConfigKey key);
    private:
        /// Sound: When the game is paused
        Mix_Chunk* au_pause;
        Uint8* keystate;
        bool keypressed[SDLK_LAST];
    private:
        SDL_Event event;
        /// Check for keyboard events in game
        inline void pollGameEvents();
        /// Check for keyboard events in paused game
        inline void pollPausedEvents();
        /// Check for keyboard events in menu
        inline void pollMenuEvents();
};

#endif
