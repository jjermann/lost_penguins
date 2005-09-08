#include "objects_common.h"

/** \brief Teleporter

    Teleports players to the exit position
*/
class Teleporter : public Object {
    public:
        Teleporter(Sint16 xpos=0, Sint16 ypos=0, ParameterMap& parameters=ParameterMap());
        virtual ~Teleporter();
        static ParameterMap default_parameters;
        virtual bool act(Object* obj);
    private:
        /// \brief Exit position
        ///
        /// Bottom Center of the exit position
        SDL_Rect exit;
        Mix_Chunk* au_tele;
};
