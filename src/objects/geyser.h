#include "objects_common.h"

#define T_GEYSIR      3000
#define T_GEYSIR_DUR  1000


class Geyser : public Object {
    public:
        Geyser(string imagename, Sint16 xpos=0, Sint16 ypos=0, Sint16 yAdd=0, string name="Geyser");
        virtual ~Geyser();
        virtual void idle(Uint16 dt);
    private:
        Sint16 aspeed;
        Mix_Chunk* au_geyser;
        Uint16 Deffect;
};
