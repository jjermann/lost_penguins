#include "objects_common.h"

/** \brief Bomb item

    Becomes a dangerous bomb character when used exploding after a certain
    countdown period.
    \todo How to create TriggeredBomb when we use dynamic plugins, etc? (ok for now)
*/
class Bomb : public Item {
    public:
        Bomb(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param=ParameterMap());
        virtual ~Bomb();
        static ParameterMap default_parameters;
        virtual bool act(Object* obj);
    private:
        ParameterMap triggered_bomb_param;
};
