#include "objects_common.h"     


/** \brief Trigger

*/
class Trigger : public Object {
    public:
        Trigger(string imagename, Sint16 xcord=0, Sint16 ycord=0, string targetname="DefaultTarget", string oname="Trigger", string keyname="");
        virtual ~Trigger();
        virtual bool act(Object* obj);
    private:
        Object* target;
        string key;
};
