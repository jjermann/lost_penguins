#include "objects_common.h"     

/** \brief Wall

*/
class Wall : public Object {
    public:
        Wall(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param=ParameterMap());
        virtual ~Wall();
        static ParameterMap default_parameters;
};
