#include "monsters_common.h"

#define DE_ATT1        400

 
/** \brief Zombie

*/
class Zombie : public Monster {
    public:
        Zombie(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param=ParameterMap());
        virtual ~Zombie();
        static ParameterMap default_parameters;
        virtual void idle(Uint16);
    private:
        virtual void runAI(Uint16);
        virtual void ai_left(Uint16);
        virtual void ai_right(Uint16);
        virtual void ai_attack(Player*, Uint16);
        Mix_Chunk* au_attack;
        Uint16 T_Attack_Bite;
};
