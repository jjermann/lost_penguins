#include "characters_common.h"

/** \brief Regrowing plant

*/
class Spike : public Object {
    public:
        Spike(string imagename, Sint16 xpos=0, Sint16 ypos=0, Uint16 dir=DIR_UP, string name="Spike");
        virtual ~Spike();
        virtual void touch(Object*);
    private:
        Weapon weapon;
        Uint16 dir;
};
