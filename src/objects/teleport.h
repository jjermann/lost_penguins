#ifndef _OBJECTS_COMMON_H_
    #include "objects_common.h"
#endif

class Teleporter : public Object {
    public:
        Teleporter(string imagename, Uint16 xpos=0, Uint16 ypos=0, Uint16 xExit=0, Uint16 yExit=0, string name="Teleporter");
        virtual ~Teleporter();
        virtual bool act(Object* obj);
    private:
        SDL_Rect exit;
        Mix_Chunk* au_tele;
};
