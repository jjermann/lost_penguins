#ifndef _OBJECTS_COMMON_H_     
    #include "objects_common.h"     
#endif

class Wall : public Object {
    public:
        Wall(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Wall");
        virtual ~Wall();
};
