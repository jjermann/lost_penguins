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
#define INPUTR_LEFT     0x00010000
#define INPUTR_RIGHT    0x00020000
#define INPUTR_UP       0x00040000
#define INPUTR_DOWN     0x00080000
#define INPUTR_SP1      0x00100000
#define INPUTR_SP2      0x00200000
#define INPUTR_ACT      0x00400000
#define INPUTR_USE      0x00800000
#define INPUTR_DEL      0x01000000
#define INPUT_PAUSE     0x10000000
#define INPUT_MENU      0x20000000

/** \brief Handels keyboard events

    \remark A key release event which is passed on to the PhysicHandler
      mustn't be cummulative (see player code as well). Ie. If the event
      is triggered twice it should do the same as if once... 
*/
class InputHandler {
    public:
        InputHandler();
        ~InputHandler();
        /// Check for keyboard events in game
        void pollEvents();
        /// Check for keyboard events in menu
        void pollMEvents();
        bool getState(Uint32 cstate) {
            return (state&cstate);
        }
        void setState(Uint32 cstate) {
            state|=cstate;
        }
        void unsetState(Uint32 cstate) {
            state&=~cstate;
        }
        void clearStates() {
            state=NOTHING;
        }
    private:
        /// Information about which buttons are pressed
        Uint32 state;
        /// Sound: When the game is paused
        Mix_Chunk* au_pause;
};

/// \obsolete This isn't used at the moment
/// \brief SDL event filter
int filterEvents(const SDL_Event *event);

#endif
