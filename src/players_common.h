#ifndef _PLAYERS_COMMON_H
#define _PLAYERS_COMMON_H 1

#include "characters_common.h"

#define MAX_ITEMS       3
   
//Player states
//movement (while up/down/left/right key is pressed)
#define STATE_MLEFT     0x00000004
#define STATE_MRIGHT    0x00000008
#define STATE_MUP       0x00000010
#define STATE_MDOWN     0x00000020

/** \brief Interactively controlled characters

*/
class Player : public Character {
    public:
        Player(string img, Sint16 xpos=0, Sint16 ypos=0, string name="Player");
        virtual ~Player();
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
        virtual void updateAnimState(bool change=true);
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        virtual Uint16 hit(Uint16 direction, Weapon& weap);
        /// Removes all interactively initiated events and states
        /// \param reset If true reset the states of this player
        virtual void clearStates(bool reset=false);
        //@}
        //Input methods
        //@{
        /// \brief Called when using the right arrow (move right)
        virtual void in_right(Sint16);
        /// \brief Called when using the left arrow (move left)
        virtual void in_left(Sint16);
        /// \brief Called when using the up arrow (move up)
        virtual void in_up(Sint16);
        /// \brief Called when using the down arrow (move down)
        virtual void in_down(Sint16);
        /// \brief Called when using the space bar (special 1)
        virtual void in_sp1(Sint16);
        /// \brief Called when using the left shift key (special 2)
        virtual void in_sp2(Sint16);
        /// \brief Called when using the enter key (activate)
        ///
        /// This should be the same for all players.
        virtual void in_act(Sint16);
        /// \brief Called when using the insert key (use)
        virtual void in_use(Sint16);
        //@}
    protected:
        //@{
        virtual void addTouch(std::set<Object *>&);
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
        virtual void crash(Uint16 dir=DIR_DOWN);
        /// \brief Called when this player dies
        virtual void die();
        virtual Hit move(Uint16 dt, bool check=false);
        //@{
        Animation* im_left;
        Animation* im_right;
        Animation* im_fall_left;
        Animation* im_fall_right;
        Animation* im_krit_left;
        Animation* im_krit_right;
        Animation* im_land_left;
        Animation* im_land_right;
        //@}
        //@{
        Mix_Chunk*  au_hit;
        Mix_Chunk*  au_land;
        Mix_Chunk*  au_act;
        Mix_Chunk*  au_newitem;
        Mix_Chunk*  au_die;
        Mix_Chunk*  au_elec;
        Mix_Chunk*  au_drown;
        Mix_Chunk*  au_fire;
        Mix_Chunk*  au_heal;
        //@}
        //@{
        Item* items[MAX_ITEMS];
        Uint8 currentitem;
        //@}
};


/** \brief Dead player

    Used to draw the last animation of a dying player: The player is removed instantly and
    replaced by a DeadPlayer character that gets assigned a die CAnimationEvent.
*/
class DeadPlayer : public Character {
    public:
        DeadPlayer(string imagename, Sint16 xcord=0, Sint16 ycord=0, string name="DeadPlayer");
        virtual ~DeadPlayer();
};

#endif
