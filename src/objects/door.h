#include "objects_common.h"

/** \brief Door

*/
class Door : public Object {
    public:
        Door(Sint16 xcord=0, Sint16 ycord=0, ParameterMap& parameters=ParameterMap());
        virtual ~Door();
        static ParameterMap default_parameters;
        virtual bool act(Object* obj);
    private:
        bool open;
        string key;
        Mix_Chunk* au_open;
        Mix_Chunk* au_close;
};
