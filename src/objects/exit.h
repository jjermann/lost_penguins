#ifndef _OBJECTS_COMMON_H_
    #include "objects_common.h"
#endif


class Exit : public Object {
    public:
        Exit(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Exit");
        virtual ~Exit();
        virtual bool act(Object* obj);
};
