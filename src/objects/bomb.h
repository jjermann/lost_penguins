#ifndef _OBJECTS_COMMON_H_
    #include "objects_common.h"
#endif

class Bomb : public Item {
    public:
        Bomb(string imagename, Uint16 xpos=0, Uint16 ypos=0, Uint16 explode_time=3000, string name="Bomb");
        virtual ~Bomb();
        virtual bool act(Object* obj);
    private:
        Uint16 countdown;
        Mix_Chunk* au_bomb;
};
