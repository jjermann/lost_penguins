#ifndef _EVENTS_H
#define _EVENTS_H 1

#define EV_DELAY     0x00000001
#define EV_START     0x00000002
#define EV_RUN       0x00000004
#define EV_END       0x00000008
#define EV_CANCEL    0x00000010

//ALREADY DEFINED!!! fix this
#define STATE_RUN        0x00400000

class Event {
    public:
        Event(Object* obj, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING);
        virtual ~Event();
        virtual Uint16 update(Uint16 dt);   
        virtual void cancel();
        virtual void start();
        virtual void end();
        virtual void reset() { }
    protected:
        Object* owner;
        Uint16 duration;
        Uint16 delay;
        Sint16 tleft;
        bool started;
        Uint32 state;
};

class CEvent : public Event {
    public:
        CEvent(Character* chr, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING);
    protected:
        Character* charowner;
};

class AnimEvent : public Event {
    public:
        AnimEvent(Object* obj, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING, Mix_Chunk* asound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual Uint16 update(Uint16 dt);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        Animation* anim;
        bool del;
        Mix_Chunk* sound;
};
  
class CAnimEvent : public CEvent {
    public:
        CAnimEvent(Character* chr, Uint16 length, Uint16 edelay=0, Uint32 switchstate=NOTHING, Mix_Chunk* asound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual Uint16 update(Uint16 dt);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        Animation* anim;
        bool del;
        Mix_Chunk* sound;
};

class EAttack : public CAnimEvent {
    public:
        EAttack(Character* chr, Uint16 length, Weapon* atweapon, Uint16 dir, Uint16 weapon_range=0, Uint16 target_mask=NOTHING, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual void end();
    protected:
        Weapon* weapon;
        Uint16 direction;
        Uint16 range;
        Uint16 mask;
};

class ESpeed : public CAnimEvent {
    public:
        ESpeed(Character* chr, Uint16 length, Sint16 avspeed, Sint16 ahspeed=0, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual void start();
        virtual void end();  
        virtual void cancel();
    protected:
        Sint16 vspeed;
        Sint16 hspeed;
};

class ERun : public ESpeed {
    public:
        ERun(Character* chr, Uint16 length, Sint16 inispeed, Sint16 ahspeed, Uint16 edelay=0, Uint32 switchstate=0, Mix_Chunk* esound=NULL, Animation* runanim=NULL, bool delanim=false);
        virtual ~ERun();
        virtual void reset(); 
        virtual Uint16 update(Uint16 dt);
    protected:
        Sint16 ispeed;
        Sint16 t_reset;
};

#endif
