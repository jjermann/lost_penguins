#include "objects_common.h"     


/** \brief Trigger

    Always add triggers after their related objects!
*/
class Trigger : public Object {
    public:
        Trigger(Sint16 xcord=0, Sint16 ycord=0, const ParameterMap& param=ParameterMap());
        virtual ~Trigger();
        static ParameterMap default_parameters;
        virtual bool act(Object* obj);
    private:
        Object* target;
        string key;
};
