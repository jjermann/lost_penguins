#include "objects_common.h"     


/** \brief Wind

*/
class Wind : public Object {
    public:
        Wind(Sint16 xpos=0, Sint16 ypos=0, ParameterMap& parameters=ParameterMap());
        virtual ~Wind();
        static ParameterMap default_parameters;
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
    private:
        Sint16 gravitymod;
};
