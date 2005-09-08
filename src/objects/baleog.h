#include "players_common.h"

/** \brief Baleog (Viking)

*/
class Baleog : public Player {
    public:
        Baleog(Sint16 xpos=0, Sint16 ypos=0, ParameterMap& parameters=ParameterMap());
        virtual ~Baleog();
        static ParameterMap default_parameters;
        /// \brief Baleog attacks with a sword
        virtual void in_sp1();
    private:
        Mix_Chunk* au_sword;
        EmptyAnimationPtr anim_baleog_sword_left;
        EmptyAnimationPtr anim_baleog_sword_right;
};
