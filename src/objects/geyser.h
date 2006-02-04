#include "objects_common.h"

#define T_GEYSIR      3000
#define T_GEYSIR_DUR  1000


/** \brief Geyser

*/
class Geyser : public Object {
    public:
        Geyser(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param=ParameterMap());
        virtual ~Geyser();
        static ParameterMap default_parameters;
        virtual void idle(Uint16 dt);
    private:
        Sint16 aspeed;
        Mix_Chunk* au_geyser;
        Uint16 Deffect;
};
