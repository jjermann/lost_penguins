#include "objects_common.h"


/** \brief Healing item

    Heals the player by one point when used.
*/
class Heart : public Item {
    public:
        Heart(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Heart");
        virtual ~Heart();
        virtual bool act(Object* obj);
};
