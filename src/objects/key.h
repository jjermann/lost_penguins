#ifndef _OBJECTS_COMMON_H_
    #include "objects_common.h"
#endif


class Key : public Item {
    public:
        Key(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Key");
        virtual ~Key();
        virtual bool act(Object* obj);
    private:
        Mix_Chunk* au_key;
};
