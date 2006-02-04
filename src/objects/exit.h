#include "objects_common.h"


/** \brief Exit

*/
class Exit : public Object {
    public:
        Exit(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param =ParameterMap());
        virtual ~Exit();
        static ParameterMap default_parameters;
        virtual bool act(Object* obj);
};
