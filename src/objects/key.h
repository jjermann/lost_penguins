#include "objects_common.h"


/** \brief Key item

    Can be used to unlock a door by using it with a Lock trigger.  
*/
class Key : public Item {
    public:
        Key(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Key");
        virtual ~Key();
        virtual bool act(Object* obj);
    private:
        Mix_Chunk* au_key;
};
