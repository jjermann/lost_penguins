#include "characters_common.h"

/** \brief Exploding Bomb

*/
class TriggeredBomb : public Character {
    public:
        TriggeredBomb(Sint16 xpos=0, Sint16 ypos=0, ParameterMap& parameters=ParameterMap());
        virtual ~TriggeredBomb();
        static ParameterMap default_parameters;
        virtual void idle(Uint16);
    private:
        Sint16 countdown;
        Mix_Chunk* au_bomb;
};
