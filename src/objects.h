//Continued list of states
#define STATE_SWIM_S     0x00010000
#define STATE_CLIMB_L    0x00010000
#define STATE_SWIM_F     0x00020000
#define STATE_CLIMB_R    0x00020000
//STATE_CLIMB_L|STATE_CLIMB_R
#define STATE_CLIMB      0x00030000
#define STATE_SHIELD     0x00040000
#define STATE_GLIDE      0x00040000
#define STATE_JUMP_OFF_R 0x00080000
#define STATE_JUMP_OFF_L 0x00100000
#define STATE_JUMP_OFF   0x00180000
#define STATE_SMALL      0x00200000
#define STATE_RUN        0x00400000

//Eric
#define V_JUMP        -400
#define V_JUMP2       -500
//Olaf
#define V_SHIELD       100
#define V_FART        -400
//time from falling start till Olaf changes from small to big
#define V_SMALLCHANGE  400
//Scorch
#define SCORCH_MAX_WINGS 5
#define V_FLY         -400
//maximal upwards velocity
#define V_MIN         -600
#define V_GLIDE        100
//Fang
#define V_JUMPOFF     -600
#define V_CLIMB        100
#define T_JUMPOFF      500

#define T_GEYSIR      3000
#define T_GEYSIR_DUR  1000

//Events
#define DE_JUMP        200
#define DE_WING        300
#define DE_ATT1        400

 
/*==================
 Common Characters
==================*/
class TriggeredBomb : public Character {
    public:
        TriggeredBomb(string imagename, Uint16 xpos=0, Uint16 ypos=0, Uint16 dleft=3000, string name="TriggeredBomb");
        virtual ~TriggeredBomb();
        virtual void idle(Uint16);
    private:
        Mix_Chunk* au_bomb;
        Sint16 countdown;
};


/*==================
      Vikings
==================*/
class Eric : public Viking {
    public:
        Eric(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Eric");
        virtual ~Eric();
        virtual void in_sp1(Uint16);
        virtual void in_sp2(Uint16);
    private:
        Mix_Chunk* au_jump;
        Mix_Chunk* au_run;
        Sint16 jump,jump2;
};

class Olaf : public Viking {
    public:
        Olaf(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Olaf");
        virtual ~Olaf();
        virtual void updateAnimState();
        virtual void in_left(Uint16);
        virtual void in_right(Uint16);
        virtual void in_down(Uint16);
        virtual void in_up(Uint16);
        virtual void fall(Uint16);
        virtual void in_sp1(Uint16);
        virtual void in_sp2(Uint16);
        virtual Sint16 hit(Uint16,Weapon&);
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

class Baleog : public Viking {
    public:
        Baleog(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Baleog");
        virtual ~Baleog();
    private:
        Mix_Chunk* au_hit;
};

class Fang : public Viking {
    public:
        Fang(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Fang");
        virtual ~Fang();
        virtual void fall(Uint16);
        virtual void in_left(Uint16);
        virtual void in_right(Uint16);
        virtual void in_sp1(Uint16);
    private:
        virtual void land();
        Mix_Chunk* au_jump;
        Sint16 jump;
};

class Scorch : public Viking {
    public:
        Scorch(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Scorch");
        virtual ~Scorch();
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        virtual void in_sp1(Uint16);
    private:
        virtual void land();
        Uint8 left_wings;
        Mix_Chunk* au_swing;
        Mix_Chunk* au_tired;
        Sint16 wing;
};


/*==================
      Monsters
==================*/

class Zombie : public Monster {
    public:
        Zombie(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Zombie");
        virtual ~Zombie();
        virtual void idle(Uint16);
    private:
        virtual void runAI(Uint16);
        virtual void ai_left(Uint16);
        virtual void ai_right(Uint16);
        virtual void ai_attack(Viking*, Uint16);
        Uint16 T_Attack_Bite;
};


/*==================
   Common Objects
==================*/

class Wall : public Object {
    public:
        Wall(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Wall");
        virtual ~Wall();
};

class Exit : public Object {
    public:
        Exit(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Exit");
        virtual ~Exit();
        virtual bool act(Object* obj);
};

class Water : public Object {
    public:
        Water(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Water");
        virtual ~Water();
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
};

class Wind : public Object {
    public:
        Wind(string imagename, Uint16 xpos=0, Uint16 ypos=0, Sint16 Accel=0, string name="Wind");
        virtual ~Wind();
        virtual void enter(Object* obj);
        virtual void leave(Object* obj);
    private:
        Sint16 gravitymod;
};

class Geysir : public Object {
    public:
        Geysir(string imagename, Uint16 xpos=0, Uint16 ypos=0, Sint16 yAdd=0, string name="Geysir");
        virtual ~Geysir();
        virtual void idle(Uint16 dt);
    private:
        Uint16 Deffect;
        Sint16 aspeed;
        Mix_Chunk* au_geysir;
};

class Teleporter : public Object {
    public:
        Teleporter(string imagename, Uint16 xpos=0, Uint16 ypos=0, Uint16 xExit=0, Uint16 yExit=0, string name="Teleporter");
        virtual ~Teleporter();
        virtual bool act(Object* obj);
    private:
        SDL_Rect exit;
        Mix_Chunk* au_tele;
};

class Trigger : public Object {
    public:
        Trigger(string imagename, Uint16 xcord=0, Uint16 ycord=0, string targetname="DefaultTarget", string oname="Trigger", string keyname="");
        virtual ~Trigger();
        virtual bool act(Object* obj);
    private:
        Object* target;
        string key;
};

class Door : public Object {
    public:
        Door(string imagename, Uint16 xcord=0, Uint16 ycord=0, string keyname="Key", string oname="Door");
        virtual ~Door();
        virtual bool act(Object* obj);
    private:
        bool open;
        string key;
};


/*==================
       Items
==================*/
     
class Heart : public Item {
    public:
        Heart(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Heart");
        virtual ~Heart();
        virtual bool act(Object* obj);
    private:
        Mix_Chunk* au_heal;
};

class Key : public Item {
    public:
        Key(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Key");
        virtual ~Key();
        virtual bool act(Object* obj);
    private:
        Mix_Chunk* au_key;
};

class Bomb : public Item {
    public:
        Bomb(string imagename, Uint16 xpos=0, Uint16 ypos=0, Uint16 explode_time=3000, string name="Bomb");
        virtual ~Bomb();
        virtual bool act(Object* obj);
    private:
        Uint16 countdown;
        Mix_Chunk* au_bomb;
};
