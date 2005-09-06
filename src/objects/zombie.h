#include "monsters_common.h"

#define DE_ATT1        400

 
/** \brief Zombie

*/
class Zombie : public Monster {
    public:
        Zombie(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Zombie");
        virtual void idle(Uint16);
    private:
        virtual void runAI(Uint16);
        virtual void ai_left(Uint16);
        virtual void ai_right(Uint16);
        virtual void ai_attack(Player*, Uint16);
        Mix_Chunk* au_attack;
        Uint16 T_Attack_Bite;
};
