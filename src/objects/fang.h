#include "players_common.h"

#define STATE_CLIMB_L    0x00010000
#define STATE_CLIMB_R    0x00020000
//STATE_CLIMB_L|STATE_CLIMB_R
#define STATE_CLIMB      0x00030000
#define STATE_JUMP_OFF_R 0x00080000
#define STATE_JUMP_OFF_L 0x00100000
#define STATE_JUMP_OFF   0x00180000

#define V_JUMP        -400
#define V_JUMPOFF     -600
#define V_CLIMB        100
#define T_JUMPOFF      500
#define DE_JUMP        200

 
/** \brief Fang (Viking)

*/
class Fang : public Player {
    public:
        Fang(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Fang");
        virtual ~Fang();
        virtual void fall(Uint16);
        virtual void in_left(Sint16);
        virtual void in_right(Sint16);
        virtual void in_sp1(Sint16);
        virtual void clearStates(bool reset=false);
    private:
        virtual void crash(Uint16 dir=DIR_DOWN);
        Mix_Chunk* au_jump;
        Sint16 jump;
};
