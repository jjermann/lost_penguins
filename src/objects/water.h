#include "objects_common.h"     


/** \brief Water

*/
class Water : public Object {
    public:
        Water(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Water");
        virtual ~Water();
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
    protected:
        Weapon weapon;
        Mix_Chunk* au_water;
};
