#include "objects_common.h"     


class Water : public Object {
    public:
        Water(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Water");
        virtual ~Water();
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
    protected:
        Weapon weapon;
        Mix_Chunk* au_water;
};
