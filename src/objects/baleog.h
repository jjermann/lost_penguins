#include "players_common.h"

/** \brief Baleog (Viking)

*/
class Baleog : public Player {
    public:
        Baleog(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Baleog");
        virtual ~Baleog();
        /// \brief Baleog attacks with a sword
        virtual void in_sp1();
    private:
        Mix_Chunk* au_sword;
        EmptyAnimationPtr anim_sword_left;
        EmptyAnimationPtr anim_sword_right;
};
