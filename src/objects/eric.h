#include "players_common.h"

#define STATE_RUN        0x00400000
#define V_JUMP        -400
#define V_JUMP2       -500
#define DE_JUMP        200

class Eric : public Player {
    public:
        Eric(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Eric");
        virtual ~Eric();
        virtual void in_sp1(Sint16);
        virtual void in_sp2(Sint16);
        virtual void in_left(Sint16);
        virtual void in_right(Sint16);
        virtual Uint16 hit(Uint16,Weapon&);
    private:
        Mix_Chunk* au_jump;
        Mix_Chunk* au_run;
        Sint16 jump,jump2;
};
