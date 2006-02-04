#include "characters_common.h"

/** \brief Regrowing plant

*/
class Plant : public Monster {
    public:
        Plant(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param=ParameterMap());
        virtual ~Plant();
        static ParameterMap default_parameters;
        virtual void touch(Object*);
        virtual void idle(Uint16);
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
    private:
        Uint16 recover;
        Sint16 tcur;
        Mix_Chunk* au_recover;
};
