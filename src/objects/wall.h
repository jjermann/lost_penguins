#include "objects_common.h"     

/** \brief Wall

*/
class Wall : public Object {
    public:
        Wall(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Wall");
        virtual ~Wall();
};
