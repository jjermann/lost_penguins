#include "objects_common.h"     


class Trigger : public Object {
    public:
        Trigger(string imagename, Uint16 xcord=0, Uint16 ycord=0, string targetname="DefaultTarget", string oname="Trigger", string keyname="");
        virtual ~Trigger();
        virtual bool act(Object* obj);
    private:
        Object* target;
        string key;
};
