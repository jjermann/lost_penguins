#include "objects_common.h"     


/** \brief Water

*/
class Water : public Object {
    public:
        Water(Sint16 xpos=0, Sint16 ypos=0, ParameterMap& parameters=ParameterMap());
        virtual ~Water();
        static ParameterMap default_parameters;
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
    protected:
        Weapon weapon;
        Mix_Chunk* au_water;
};
