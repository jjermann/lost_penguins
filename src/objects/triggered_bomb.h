#ifndef _CHARACTERS_COMMON_H_
    #include "characters_common.h"
#endif

class TriggeredBomb : public Character {
    public:
        TriggeredBomb(string imagename, Uint16 xpos=0, Uint16 ypos=0, Uint16 dleft=3000, string name="TriggeredBomb");
        virtual ~TriggeredBomb();
        virtual void idle(Uint16);
    private:
        Sint16 countdown;
        Mix_Chunk* au_bomb;
};
