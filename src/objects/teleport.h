#include "objects_common.h"

class Teleporter : public Object {
    public:
        Teleporter(string imagename, Sint16 xpos=0, Sint16 ypos=0, Sint16 xExit=0, Sint16 yExit=0, string name="Teleporter");
        virtual ~Teleporter();
        virtual bool act(Object* obj);
    private:
        SDL_Rect exit;
        Mix_Chunk* au_tele;
};
