#include "objects_common.h"

/** \brief Bomb item

    Becomes a dangerous bomb character when used exploding after a certain
    countdown period.  
*/
class Bomb : public Item {
    public:
        Bomb(string imagename, Sint16 xpos=0, Sint16 ypos=0, Uint16 explode_time=3000, string name="Bomb");
        virtual ~Bomb();
        virtual bool act(Object* obj);
    private:
        Uint16 countdown;
        Mix_Chunk* au_bomb;
};
