#include "objects_common.h"

/** \brief Teleporter

    Teleports players to the exit position
*/
class Teleporter : public Object {
    public:
        /// \param xExit x coordinate of the exit position
        /// \param yExit y coordinate of the exit position
        Teleporter(string imagename, Sint16 xpos=0, Sint16 ypos=0, Sint16 xExit=0, Sint16 yExit=0, string name="Teleporter");
        virtual ~Teleporter();
        virtual bool act(Object* obj);
    private:
        /// \brief Exit position
        ///
        /// Bottom Center of the exit position
        SDL_Rect exit;
        Mix_Chunk* au_tele;
};
