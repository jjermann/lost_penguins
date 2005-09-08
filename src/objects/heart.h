#include "objects_common.h"


/** \brief Healing item

    Heals the player by one point when used.
*/
class Heart : public Item {
    public:
        Heart(Sint16 xpos=0, Sint16 ypos=0, ParameterMap& parameters=ParameterMap());
        virtual ~Heart();
        static ParameterMap default_parameters;
        virtual bool act(Object* obj);
    private:
        Uint8 alife;
};
