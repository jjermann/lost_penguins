#ifndef _WEAPONS_H
#define _WEAPONS_H 1

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
#define WS_SPIKE         0x00000020
#define WS_HEAL          0x00000040

#define TSTRIKE          600

/** \brief Weapons

    Describe the type of the damage when reaching a target. It does _not_ contain
    any information about the range, direction, animation, etc!
*/
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
        /// \brief Weapon type
        ///
        /// Determines how the weapon affects other characters (logically).
        Uint16 wtype;
        /// \brief Weapon sub type
        ///
        /// Determines the kind of the weapon. It has no influence on the damage
        /// or the logical reaction and is just used to determine the visual/audio
        /// response of the target.
        Uint16 stype;
        /// \brief Weapon damage
        ///
        /// Damage the weapon deals if it reaches it isn't prevented.
        Sint16 damage;
};

#endif
