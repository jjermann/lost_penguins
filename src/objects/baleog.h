#include "players_common.h"

class Baleog : public Player {
    public:
        Baleog(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Baleog");
        virtual ~Baleog();
    private:
        Mix_Chunk* au_hit;
};
