#ifndef _OBJECTS_COMMON_H_
    #include "objects_common.h"
#endif


class Heart : public Item {
    public:
        Heart(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Heart");
        virtual ~Heart();
        virtual bool act(Object* obj);
};
