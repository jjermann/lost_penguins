#ifndef _OBJECTS_COMMON_H_     
    #include "objects_common.h"     
#endif


class Water : public Object {
    public:
        Water(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Water");
        virtual ~Water();
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
};
