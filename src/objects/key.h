#include "objects_common.h"


/** \brief Key item

    Can be used to unlock a door by using it with a Lock trigger.  
*/
class Key : public Item {
    public:
        Key(Sint16 xpos=0, Sint16 ypos=0, ParameterMap& parameters=ParameterMap());
        virtual ~Key();
        static ParameterMap default_parameters;
        virtual bool act(Object* obj);
    private:
        Mix_Chunk* au_key;
};
