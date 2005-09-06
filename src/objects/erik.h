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
        virtual void idle(Uint16); 
       /// \brief Erik jumps
        virtual void in_sp1();
        /// \brief Erik runs
        virtual void in_sp2();
        virtual void in_left(Uint16);
        virtual void in_right(Uint16);
        virtual void crash(Uint16 dir);
        virtual Uint16 hit(Uint16 direction,Weapon& weap);
    private:
        EmptyAnimationPtr anim_erik_jump_left;
        EmptyAnimationPtr anim_erik_jump_right;
        EmptyAnimationPtr anim_erik_jump2_left;
        EmptyAnimationPtr anim_erik_jump2_right;
        EmptyAnimationPtr anim_erik_start_run_left;
        EmptyAnimationPtr anim_erik_start_run_right;
        EmptyAnimationPtr anim_erik_swim_left;
        EmptyAnimationPtr anim_erik_swim_right;
        EmptyAnimationPtr anim_erik_swim_up_left;
        EmptyAnimationPtr anim_erik_swim_up_right;
        EmptyAnimationPtr anim_erik_hit_water_left;
        EmptyAnimationPtr anim_erik_hit_water_right;
        Mix_Chunk* au_jump;
        Mix_Chunk* au_run;
        Sint16 jump,jump2;
        bool run_right;
};
