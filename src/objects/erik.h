#include "players_common.h"

#define V_JUMP        -400
#define V_JUMP2       -500
#define DE_JUMP        200

/** \brief Erik (Viking)

    Act1: first jump\n
    Act2: second jump
    \todo Implement water (STATE_WATER)
    \todo Add jump and run attacks
*/
class Erik : public Player {
    public:
        Erik(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Erik");
        virtual ~Erik();
        /// \brief Erik jumps
        virtual void in_sp1(Sint16);
        // \brief Erik runs
        virtual void in_sp2(Sint16);
        virtual void in_left(Sint16);
        virtual void in_right(Sint16);
        virtual void crash(Uint16 dir);
        virtual Uint16 hit(Uint16 direction,Weapon& weap);
    private:
        Mix_Chunk* au_jump;
        Mix_Chunk* au_run;
        Sint16 jump,jump2;
};
