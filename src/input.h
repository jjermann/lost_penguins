#ifndef _INPUT_H_
    #define _INPUT_H_
#endif

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

class InputHandler {
    public:
        InputHandler();
        ~InputHandler();
        //check for keyboard events
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
        int filterEvents(const SDL_Event *event);
        Uint16 state;
        Mix_Chunk* au_pause;
};

//filter events
int filterEvents(const SDL_Event *event);
