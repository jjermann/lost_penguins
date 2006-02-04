#ifndef DEF_PLAYERS_COMMON_H
#define DEF_PLAYERS_COMMON_H 1

#include "characters_common.h"

#define MAX_ITEMS       3
   
//Player states
//movement (while up/down/left/right key is pressed)
#define STATE_MLEFT     0x00000400
#define STATE_MRIGHT    0x00000800
#define STATE_MUP       0x00001000
#define STATE_MDOWN     0x00002000

/** \brief Interactively controlled characters

*/
class Player : public Character {
    public:
        Player(Sint16 xpos=0, Sint16 ypos=0, const ParameterMap& param=ParameterMap());
        virtual ~Player();
        static ParameterMap default_parameters;
        //@{
        /// \brief Returns the icon of this Player
        /// \return Single icon frame of this player (used for the player bar)
        Frame getIcon() const;
        /// \brief Returns the current icon number
        /// \return Current icon number
        Uint8 getItemNum() {
            return currentitem;
        }
        Item* getItem(Uint8 num) {
            if (num<MAX_ITEMS) return items[num];
            else return NULL;
        }
        /// Inserts a detached Item (usually from the ObjectsPool) and tries to add
        /// it to the item pool of this player.
        /// \return True if the item could be added
        bool pickupItem(Item* newitem);
        /// Detaches an item from the item pool
        /// \return Detached Item
        Item* moveItem() {
            return moveItem(currentitem);
        }
        /// Detaches and removes an Item from the item pool
        void removeItem() {
            removeItem(currentitem);
        }
        /// Selects a new current item from the item pool
        /// \param right If true take the next item to the right, otherwise the
        ///        next to the left
        void switchItem(bool right=true);
        /// Detaches an item and adds it back to the ObjectsPool
        Object* dropItem() {
            return dropItem(currentitem);
        }
        //@}
        //VIRTUAL METHODS
        //@{
        virtual void updateAnimState();
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
        /// Removes all interactively initiated events and states
        /// \param reset If true reset the states of this player
        virtual void clearStates(bool reset=false);
        virtual void setEvent(Event* ev);
        //@}
        //Input methods
        //@{
        /// \brief Called when using the move right key (right arrow)
        virtual void in_right() { }
        virtual void in_right(Uint16);
        /// \brief Called when using the move left key (left arrow)
        virtual void in_left() { }
        virtual void in_left(Uint16);
        /// \brief Called when using the move up key (up arrow)
        virtual void in_up() { }
        virtual void in_up(Uint16) { }
        /// \brief Called when using the move down key (down arrow)
        virtual void in_down() { }
        virtual void in_down(Uint16) { }
        /// \brief Called when using the special 1 key (space)
        virtual void in_sp1() { }
        /// \brief Called when using the special 2 key (left shift)
        virtual void in_sp2() { }
        /// \brief Called when using the activation key (enter)
        ///
        /// This should be the same for all players.
        virtual void in_act();
        /// \brief Called when using the use key (insert)
        virtual void in_use();
        //@}
    protected:
        //@{
        virtual void addTouch(std::set<Object *>&);
        /// Runs Character::addEnter() and tries to pickup any Items
        /// it finds
        /// \todo dynamic_casts are not nice solutions
        virtual void addEnter(std::set<Object *>&);
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        //@}
        //@{
        void removeItem(Uint8 num);
        Item* moveItem(Uint8 num);
        Object* dropItem(Uint8 num);
        //@}
        /// \brief Called when crashing into a dense object (default: ground)
        /// \param dir Direction of the crash
        /// \bug crash is called _after_ addEnter/etc, so if one of those sets
        ///   an event, it will be canceled by crash!
        virtual void crash(Uint16 dir=DIR_DOWN);
        /// \brief Called when this player dies
        virtual void die();
        virtual Hit move(Uint16 dt, bool check=false);
        //@{
        Item* items[MAX_ITEMS];
        Uint8 currentitem;
        //@}
        //@{
        Mix_Chunk*  au_hit;
        Mix_Chunk*  au_land;
        Mix_Chunk*  au_act;
        Mix_Chunk*  au_useerror;
        Mix_Chunk*  au_newitem;
        Mix_Chunk*  au_die;
        Mix_Chunk*  au_elec;
        Mix_Chunk*  au_drown;
        Mix_Chunk*  au_fire;
        Mix_Chunk*  au_heal;
        //@}
        //@{
        EmptyAnimationPtr anim_left;
        EmptyAnimationPtr anim_right;
        EmptyAnimationPtr anim_rock_left;
        EmptyAnimationPtr anim_rock_right;
        EmptyAnimationPtr anim_walk_left;
        EmptyAnimationPtr anim_walk_right;
        EmptyAnimationPtr anim_push_left;
        EmptyAnimationPtr anim_push_right;
        EmptyAnimationPtr anim_fall_left;
        EmptyAnimationPtr anim_fall_right;
        EmptyAnimationPtr anim_fall_fast_left;
        EmptyAnimationPtr anim_fall_fast_right;
        EmptyAnimationPtr anim_crash_left;
        EmptyAnimationPtr anim_crash_right;
        EmptyAnimationPtr anim_rope_left;
        EmptyAnimationPtr anim_rope_right;
        EmptyAnimationPtr anim_teleport_left;
        EmptyAnimationPtr anim_teleport_right;
        EmptyAnimationPtr anim_die_crash_left;
        EmptyAnimationPtr anim_die_crash_right;
        EmptyAnimationPtr anim_die_burn_left;
        EmptyAnimationPtr anim_die_burn_right;
        EmptyAnimationPtr anim_die_bones_left;
        EmptyAnimationPtr anim_die_bones_right;
        EmptyAnimationPtr anim_die_elec_left;
        EmptyAnimationPtr anim_die_elec_right;
        EmptyAnimationPtr anim_die_spike_left;
        EmptyAnimationPtr anim_die_spike_right;
        EmptyAnimationPtr anim_die_water_left;
        EmptyAnimationPtr anim_die_water_right;
        EmptyAnimationPtr anim_fall_middle;
        EmptyAnimationPtr anim_climb;
        EmptyAnimationPtr anim_bar;
        //@}
};


/** \brief Dead player

    Used to draw the last animation of a dying player: The player is removed instantly and
    replaced by a DeadPlayer character that gets assigned a die CAnimationEvent.
*/
class DeadPlayer : public Character {
    public:
        DeadPlayer(Sint16 xcord=0, Sint16 ycord=0, const ParameterMap& param=ParameterMap());
        virtual ~DeadPlayer();
        static ParameterMap default_parameters;
};

#endif
