#include "objects_common.h"     

class Wall : public Object {
    public:
        Wall(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Wall");
        virtual ~Wall();
};
