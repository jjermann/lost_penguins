#include "characters_common.h"

/** \brief Regrowing plant

*/
class Plant : public Monster {
    public:
        Plant(string imagename, Sint16 xpos=0, Sint16 ypos=0, Uint16 dleft=3000, string name="Plant");
        virtual ~Plant();
        virtual void touch(Object*);
        virtual void idle(Uint16);
        virtual Uint16 hit(Uint16 dir, Weapon& weap);
    private:
        Uint16 recover;
        Sint16 tcur;
        Mix_Chunk* au_recover;
};
