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

class ELand : public CharacterEvent {
    public:
        ELand(Character* chr, Uint16 length, Mix_Chunk* esound=NULL);
        virtual void start();
};

class ESpeed : public CharacterEvent {
    public:
        ESpeed(Character* chr, Uint16 length, Sint16 ajspeed, Sint16 ahspeed=0, Sint16 avspeed=0, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL);
        virtual void start();
        virtual Uint16 update(Uint16 dt);
    protected:
        Sint16 jspeed;
        Sint16 hspeed;
        Sint16 vspeed;
};
