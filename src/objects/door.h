#ifndef _OBJECTS_COMMON_H_
    #include "objects_common.h"
#endif

class Door : public Object {
    public:
        Door(string imagename, Uint16 xcord=0, Uint16 ycord=0, string keyname="Key", string oname="Door");
        virtual ~Door();
        virtual bool act(Object* obj);
    private:
        bool open;
        string key;
        Mix_Chunk* au_open;
        Mix_Chunk* au_close;
};
