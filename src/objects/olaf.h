#include "players_common.h"

#define STATE_SHIELD     0x00040000
#define STATE_SMALL      0x00200000
#define V_SHIELD       100
#define V_FART        -400
//time from falling start till Olaf changes from small to big
#define V_SMALLCHANGE  400
#define DE_JUMP        200

 
/** \brief Olaf (Viking)

    Act1: Farted\n
    Act2: Farted from ground
*/
class Olaf : public Player {
    public:
        Olaf(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Olaf");
        virtual ~Olaf();
        /// Additionally checks if Olaf is small and how he wears his shield
        virtual void updateAnimState(bool change=true);
        virtual void in_left(Sint16);
        virtual void in_right(Sint16);
        /// \brief Olaf tries to shrink (if he was big)
        virtual void in_down(Sint16);
        /// \brief Olaf tries to get big again (if he was small)
        virtual void in_up(Sint16);
        virtual void fall(Uint16);
        /// \brief Olaf switches his shield state if possible
        virtual void in_sp1(Sint16);
        /// \brief Olaf farts
        virtual void in_sp2(Sint16);
        virtual Uint16 hit(Uint16 direction,Weapon& weap);
    private:
        /// \brief Tries to change the size
        ///
        /// Tries to change the size according to the specified parameter.
        /// \param small If true Olaf tries to shrink, otherwise he tries
        ///   to get big
        /// \return True if successfull
        inline bool trySmall(bool small);
        //@{
        Animation* im_small_left;
        Animation* im_small_right;
        Animation* im_shield_left;
        Animation* im_shield_right;
        Animation* im_fall_shield_left;
        Animation* im_fall_shield_right;
        //@}
        //@{
        Mix_Chunk* au_small;
        Mix_Chunk* au_big;
        Mix_Chunk* au_fart;
        //@}
        Sint16 fart;
};
