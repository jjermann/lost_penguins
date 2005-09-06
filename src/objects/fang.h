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

    Act1: jump
    \todo Add claw attack
*/
class Fang : public Player {
    public:
        Fang(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Fang");
        virtual void fall(Uint16);
        virtual void in_left(Uint16);
        virtual void in_right(Uint16);
        /// \brief Fang jumps
        virtual void in_sp1();
        /// \brief Fang attacks
        virtual void in_sp2();
        virtual void clearStates(bool reset=false);
    private:
        virtual void crash(Uint16 dir=DIR_DOWN);
        EmptyAnimationPtr anim_claw_left;
        EmptyAnimationPtr anim_claw_right;
        Mix_Chunk* au_jump;
        Mix_Chunk* au_claw;
        Sint16 jump;
};
