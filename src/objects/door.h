#include "objects_common.h"

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
