#ifndef _VIKINGS_COMMON_H_
    #include "vikings_common.h"
#endif

class Baleog : public Viking {
    public:
        Baleog(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Baleog");
        virtual ~Baleog();
    private:
        Mix_Chunk* au_hit;
};
