#ifndef _INPUT_H
#define _INPUT_H 1

//Input events
#define INPUT_LEFT      0x00000001
#define INPUT_RIGHT     0x00000002
#define INPUT_UP        0x00000004
#define INPUT_DOWN      0x00000008
#define INPUT_SP1       0x00000010
#define INPUT_SP2       0x00000020
#define INPUT_ACT       0x00000040
#define INPUT_USE       0x00000080
#define INPUT_DEL       0x00000100
#define INPUT_PAUSE     0x10000000

/** \brief Handels keyboard events

*/
class InputHandler {
    public:
        InputHandler();
        ~InputHandler();
        /// Check for keyboard events
        void pollEvents();
        bool getState(Uint16 cstate) {
            return (state&cstate);
        }
        void setState(Uint16 cstate) {
            state|=cstate;
        }
        void unsetState(Uint16 cstate) {
            state&=~cstate;
        }
    private:
        /// Information about which buttons are pressed
        Uint16 state;
        /// Sound: When the game is paused
        Mix_Chunk* au_pause;
};

/// \obsolete This isn't used at the moment
/// \brief SDL event filter
int filterEvents(const SDL_Event *event);

#endif
