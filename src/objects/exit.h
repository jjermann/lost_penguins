#include "objects_common.h"


/** \brief Exit

*/
class Exit : public Object {
    public:
        Exit(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Exit");
        virtual ~Exit();
        virtual bool act(Object* obj);
};
