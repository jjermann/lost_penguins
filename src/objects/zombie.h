#ifndef _MONSTERS_COMMON_H_
    #include "monsters_common.h"
#endif

#define DE_ATT1        400

 
class Zombie : public Monster {
    public:
        Zombie(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Zombie");
        virtual ~Zombie();
        virtual void idle(Uint16);
    private:
        virtual void runAI(Uint16);
        virtual void ai_left(Uint16);
        virtual void ai_right(Uint16);
        virtual void ai_attack(Player*, Uint16);
        Uint16 T_Attack_Bite;
        Mix_Chunk* au_attack;
};
