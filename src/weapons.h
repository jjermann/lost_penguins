#ifndef _WEAPONS_H_
    #define _WEAPONS_H_
#endif

//normal hit
#define W_STRIKE         0x00000001
//bomb
#define W_EXPLOSION      0x00000002
//plant, tomator?
#define W_TOUCH          0x00000004
//speed hit
#define W_PRESSURE       0x00000008
//food, remove this?
#define W_HEAL           0x00000010
//water
#define W_WATER          0x00000020

//Weapons subtypes
#define WS_NORMAL        0x00000001
#define WS_ELECTRIC      0x00000002
#define WS_FIRE          0x00000004
#define WS_WATER         0x00000008
#define WS_PRESSURE      0x00000010
#define WS_HEAL          0x00000020

#define DSTRIKE          100
#define TSTRIKE          600

class Weapon {
    public:
        Weapon(Sint16 dmg=-1, Uint16 weaptype=NOTHING, Uint16 subtype=WS_NORMAL);
        virtual ~Weapon();
        Uint16 getType() {
            return wtype; 
        }
        Uint16 getSubType() {
            return stype;
        }
        Sint16 getDamage() {
            return damage;  
        }
    private:
        Uint16 wtype;
        Uint16 stype;
        Uint16 damage;
};
