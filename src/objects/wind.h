#include "objects_common.h"     


class Wind : public Object {
    public:
        Wind(string imagename, Uint16 xpos=0, Uint16 ypos=0, Sint16 Accel=0, string name="Wind");
        virtual ~Wind();
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
    private:
        Sint16 gravitymod;
};
