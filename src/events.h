#define EV_DELAY     0x00000001
#define EV_START     0x00000002
#define EV_RUN       0x00000004
#define EV_END       0x00000008

class CharacterEvent : public Event {
    public:
        CharacterEvent(Character* chr, Uint16 length, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL);
        virtual void cancel();
        virtual void start();
        virtual void end();
    protected:
        Character* charowner;
        Uint32 state;
        Mix_Chunk* sound;
};

class EJump : public CharacterEvent {
    public:
        EJump(Character* chr, Uint16 length, Speed aspeed, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* au_jump=NULL);
        virtual void start();
    protected:
        Speed speed;
};
