#include "players_common.h"

#define STATE_GLIDE      0x00040000
#define SCORCH_MAX_WINGS 5
#define V_FLY         -400
//maximal upwards velocity
#define V_MIN         -600
#define V_GLIDE        100
#define DE_WING        300

 
/** \brief Scorch (Viking)

    Act1: Wings used (key still down)\n
    Act2: Exhausted
*/
class Scorch : public Player {
    public:
        Scorch(string imagename, Sint16 xpos=0, Sint16 ypos=0, string name="Scorch");
        virtual ~Scorch();
        virtual void fall(Uint16);
        /// \brief Scorch uses his wings
        virtual void in_sp1(Sint16);
        virtual void clearStates(bool reset=false);
    private:
        Uint8 left_wings;
        Mix_Chunk* au_swing;
        Mix_Chunk* au_tired;
        Sint16 wing;
};
