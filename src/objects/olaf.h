#ifndef _PLAYERS_COMMON_H_
    #include "players_common.h"
#endif

#define STATE_SHIELD     0x00040000
#define STATE_SMALL      0x00200000
#define V_SHIELD       100
#define V_FART        -400
//time from falling start till Olaf changes from small to big
#define V_SMALLCHANGE  400
#define DE_JUMP        200

 
class Olaf : public Player {
    public:
        Olaf(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Olaf");
        virtual ~Olaf();
        virtual void updateAnimState(bool change=true);
        virtual void in_left(Sint16);
        virtual void in_right(Sint16);
        virtual void in_down(Sint16);
        virtual void in_up(Sint16);
        virtual void fall(Uint16);
        virtual void in_sp1(Sint16);
        virtual void in_sp2(Sint16);
        virtual Uint16 hit(Uint16,Weapon&);
    private:
        inline bool trySmall(bool small);
        Animation* im_small_left;
        Animation* im_small_right;
        Animation* im_shield_left;
        Animation* im_shield_right;
        Animation* im_fall_shield_left;
        Animation* im_fall_shield_right;
        Mix_Chunk* au_small;
        Mix_Chunk* au_big;
        Mix_Chunk* au_fart;
        Sint16 fart;
};
