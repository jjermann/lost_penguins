#include "characters_common.h"

/** \brief Regrowing plant

*/
class Spike : public Object {
    public:
        Spike(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param=ParameterMap());
        virtual ~Spike();
        static ParameterMap default_parameters;
        virtual void touch(Object*);
    private:
        Weapon weapon;
        Uint16 dir;
};
