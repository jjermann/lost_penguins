#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <math.h>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>
#ifdef SDL_IMAGE
#include <SDL_image.h>
#endif
#include "font.h"

using std::string;

class Object;
class Character;
class Viking;
class Monster;
typedef std::vector<Object *>::iterator object_viterator;
typedef std::vector<Character *>::iterator character_viterator;
typedef std::vector<Viking *>::iterator viking_viterator;
typedef std::vector<Monster *>::iterator monster_viterator;
typedef std::set<Object *>::iterator object_iterator;
typedef std::set<Character *>::iterator character_iterator;
typedef std::set<Viking *>::iterator viking_iterator;
typedef std::set<Monster *>::iterator monster_iterator;

//General
#define BAR_HEIGHT     138
#define MAX_ITEMS      3
#define MAX_HEALTH     4
#define ICON_SIZE      46
#define NOTHING        0x00000000
#define ALL            0xFFFFFFFF
   
//Falling, Landing
#define T_IRR          2000
#define V_KRIT         1000
#define T_GRAV_EFFECT  10
#define T_AI_EFFECT    20
 
//Input events
#define INPUT_LEFT     0x00000001
#define INPUT_RIGHT    0x00000002
#define INPUT_UP       0x00000004
#define INPUT_DOWN     0x00000008
#define INPUT_SP1      0x00000010
#define INPUT_SP2      0x00000020
#define INPUT_ACT      0x00000040
#define INPUT_USE      0x00000080
#define INPUT_DEL      0x00000100
#define INPUT_PAUSE    0x10000000

//Viking states, used for animations as well
//facing: either left or right (not left)
#define STATE_LEFT     0x00000001
//movement: left, right or none
#define STATE_MLEFT    0x00000004
#define STATE_MRIGHT   0x00000008
//basically any state in air
#define STATE_FALL     0x00000010
//fast falling
#define STATE_FALL2    0x00000020
//Triggers
#define STATE_ACT_1     0x00000040
#define STATE_ACT_2     0x00000080
#define STATE_ACT_3     0x00000100
//in water
#define STATE_WATER     0x00000200
//irritated (landed fast or run into wall)
#define STATE_IRR       0x00000400
//attack anim
#define STATE_ATTACK    0x00000800

//Event states
//busy:  can't do anything interactively
#define ESTATE_BUSY     0x00001000
//abort: can do anything, event is canceled in case of another event
#define ESTATE_ABORT    0x00002000
//run:  default, equivalent to event != NULL
#define ESTATE_RUN      0x00004000

//Directions
#define DIR_RIGHT       0x00000001
#define DIR_LEFT        0x00000002
#define DIR_UP          0x00000004
#define DIR_DOWN        0x00000008
#define DIR_ALL         0x0000000F
#define DIR_UPR         0x00000005
#define DIR_UPL         0x00000006
#define DIR_DWR         0x00000009
#define DIR_DWL         0x0000000A
#define DIR_LR          0x00000003

//Object types
#define OTYPE_DENSE_R   0x00000001
#define OTYPE_DENSE_L   0x00000002
#define OTYPE_DENSE_U   0x00000004
#define OTYPE_DENSE_D   0x00000008
#define OTYPE_DENSE     0x0000000F
#define OTYPE_TOUCH     0x00000020
#define OTYPE_ENTER     0x00000040
#define OTYPE_CHARACTER 0x00000080
#define OTYPE_VIKING    0x00000100
#define OTYPE_MONSTER   0x00000200
#define OTYPE_ITEM      0x00000400
#define OTYPE_PROJ      0x00000800


//Class types


//Direction
struct Hit {
   Uint16 enter;
   Uint16 touch;
};

//Weapons
class Weapon {
    public:
        Weapon(Sint16 dmg=-1, Uint16 weaptype=NOTHING, Uint16 subtype=NOTHING);
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

//Events
class Event {
    public:
        Event(Object* obj, Uint16 length, Uint16 edelay=0);
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
};


struct Config {
    Uint16 width;
    Uint16 height;
    Uint32 full;
    Uint16 audio_format;
    int audio_rate;
    int audio_channels;
    string datadir;
    string map;
};

extern Config config;
extern SDL_Rect* maparea;


struct Frame {
    SDL_Surface* image;
    //Frame position (Source Rectangle)
    SDL_Rect    pos;
};

//Animation format: a horizontal strip (image) with (num) frames
//Eventually change this to a matrix of frames: each row a different animation
class Animation {
    public:
        Animation(SDL_Surface* image, Uint16 max_num=1, Uint16 total_time=0);
        Animation(SDL_Surface* image, Uint16 width, Uint16 max_num, Uint16 total_time);
        ~Animation();
        //update a running animation
        const Frame& updateAnim(Uint16 dt);
        //update frame status of a frame series (not running)
        const Frame& setFrame(Uint16 nr);
        //return current frame
        const Frame& getFrame() const {
            return frame;
        }
        //return frame by number
        Frame getFrame(Uint16 nr) const {
            Frame newframe=frame;
            newframe.pos.x=nr*w;
            return newframe;
        }
        Uint16 getWidth() const {
            return w;
        }
        Uint16 getHeight() const {
            return h;
        }
        bool isRunning() const {
            return running;
        }
    private:
        //Frame pointer
        Frame frame;
        //Number of frames
        Uint16 size;
        //Total time (in ms)
        Uint16 time;
        //Frame size
        Uint16 w;
        Uint16 h;
        //Current frame number
        Uint16 num;
        //Current time position (in ms)
        Uint16 tcurrent;
        //frame series or running?
        bool running;
};

//Base class of almost everything
class Object {
    public:
        Object(string imagename, Uint16 xpos=0, Uint16 ypos=0, string name="Object");
        virtual ~Object();
        //uper-left
        bool setPos(Uint16 xcord,Uint16 ycord) {
            if ((xcord<=(maparea->w-pos.w)) && (ycord<=(maparea->h-pos.h))) {
                pos.x=xcord;
                pos.y=ycord;
                return true;
            } else return false;
        }
        //center
        SDL_Rect getCenter() {
            SDL_Rect tmprect;
            tmprect.x=pos.x+Uint16(pos.w/2);
            tmprect.y=pos.y+Uint16(pos.h/2);
            return tmprect;
        }
        //is the Object in the rectangle rect
        //touch: a touch (partially entered) is enough
        bool isIn(const SDL_Rect& rect, bool touch=false, Uint16 radius=0);
        //uper-left
        SDL_Rect* getPos() {
            return &pos;
        }
        //we want: bottom-middle of pos
        //animation (image) changes (depending on State changes)
        SDL_Rect* getCurPos() {
            return &curpos;
        }
        const Frame& getFrame() const {
            return animation->getFrame();
        }
        //object type
        Uint16 getType() const {
            return otype;
        }
        void setType(Uint16 newtype) {
            otype|=newtype;
        }
        void unsetType(Uint16 oldtype) {
            otype&=~oldtype;
        }
        void switchType(Uint16 newtype) {
            otype^=newtype;
        }
        string getName() {
            return name;
        }
        //update the actual Animation
        const Frame& updateAnim(Uint16 dt) {
            return (animation->updateAnim(dt));
        }
        bool isRunning() const {
            return animation->isRunning();
        }
        void mark_delete() {
            delete_flag=true;
        }
        bool isDeleted() {
            return delete_flag;
        }
        //Events (triggered animations/effects)
        void clearEvents();
        void setEvent(Event* ev);
        Uint16 updateEvents(Uint16 dt);
        void cancelEvent();
        //VIRTUAL METHODS
        //When activated by an object
        virtual bool act(Object*) { return false; }
        //When touched by an object
        virtual void touch(Object*) { }
        //When entered by an object
        virtual void enter(Object*) { }
        //When it isn't touched anymore
        virtual void untouch(Object*) { }
        //When an object leaves it
        virtual void leave(Object*) { }
        //permanent effects this base function must be run by all derived classes
        virtual void idle(Uint16) { }
        //When hit by a weapon
        virtual Sint16 hit(Uint16, Weapon&) {
            return 0;
        }
    protected:
        Event* event;
        Animation* animation;
        //temporary information about where to _draw_ the animation frame:
        //curpos.wh <= pos.wh, curpos.xy = positive dxy
        SDL_Rect curpos;
        SDL_Rect pos;
        //Object type
        Uint16 otype;
        //Object name, can be used as an id
        string name;
        //delete me flag
        bool delete_flag;
};


class Item;

//Characters
class Character : public Object {
    public:
        Character(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Character");
        virtual ~Character();
        //health
        Uint8 getHealth();
        Uint8 setHealth(Uint8);
        Uint8 addHealth(Sint8);
        //states
        bool getState(Uint32 cstate) const {
            return (state&cstate);
        }
        void setState(Uint32 cstate) {
            state|=cstate;
        }
        void switchState(Uint32 cstate) {
            state^=cstate;
        }
        void unsetState(Uint32 cstate) {
            state&=~cstate;
        }
        const std::set<Object *>& getEnter() const {
            return enter;
        }
        const std::set<Object *>& getTouch() const {
            return touch;
        }
        //Speed
        Sint16 getSpeed() const {
            return speed;
        }
        Sint16 setSpeed(Sint16 newspeed) {
            return speed=newspeed;
        }
        Sint16 addSpeed(Sint16 dspeed) {
            return speed+=dspeed;
        }
        Sint16 addHSpeed(Sint16 dspeed) {
            return hspeed+=dspeed;
        }
        void setGravity(Sint16 setgravity) {
            gravity=setgravity;
        }
        void addGravity(Sint16 addgravity) {
            gravity+=addgravity;
        }
        Hit checkMove(SDL_Rect& dest, bool tele=false, bool check=false);
        //VIRTUAL METHODS
        virtual void removedObject(Object*);
        //updates the current animation state
        virtual void updateAnimState();
        //Define these for each character, the base function must be run for all derived classes
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        //gets hit by a weapon
        virtual Sint16 hit(Uint16 direction, Weapon& weap);
    protected:
        //add places
        virtual void addTouch(std::set<Object *>&);
        virtual void addEnter(std::set<Object *>&);
        //remove places
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        //helper functions
        inline Hit checkHit(const SDL_Rect& dest, Object* destobj, bool tele=false) const;
        inline void updateRegions(std::set<Object *>& newtouch, std::set<Object *>& newenter);
        //fall -> not fall, run into wall
        virtual void land();
        //when it dies...
        virtual void die();
        //move
        virtual Hit move(Uint16 dt, bool check=false);
        //character state
        Uint32 state;
        Uint8 health;
        //maximal normal speed;
        Uint16 maxspeedx;
        Uint16 maxspeedy;
        //temporary speed (x direction)
        Sint16 hspeed;
        //current permanent speed (y direction)
        Sint16 speed;
        //current gravity
        Sint16 gravity;
        //temporary attributes
        Sint16 Dgrav,Dwater;
        //Common animations
        Animation* im_orig;
        //Entered objects
        std::set<Object *> enter;
        //Touched objects
        std::set<Object *> touch;
        //How long can the character swim?
        Uint16 t_water;
        //dense types
        Uint16 dense_types;
        //enemy types
        Uint16 enemy_types;
        //current weapon (updated for each attack)
        Weapon weapon;
    private:
        Uint16 dt_fall2;
};

//Vikings
//TODO: somehow differentiate between characters, vikings and AI players or objects
class Viking : public Character {
    public:
        Viking(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Viking");
        virtual ~Viking();
        //Items
        //returns icon for the viking bar
        Frame getIcon() const {
            return (im_right->getFrame(0));
        }
        Uint8 getItemNum() {
            return currentitem;
        }
        Item* getItem(Uint8 num) {
            if (num<MAX_ITEMS) return items[num];
            else return NULL;
        }
        bool pickupItem(Item* newitem);
        Item* moveItem() {
            return moveItem(currentitem);
        }
        void removeItem() {
            removeItem(currentitem);
        }
        void switchItem(bool right=true);
        Object* dropItem(bool right=true) {
            return dropItem(currentitem,right);
        }
        //VIRTUAL METHODS
        //updates the current animation state
        virtual void updateAnimState();
        //Define these for each viking...
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        virtual void in_right(Sint16);
        virtual void in_left(Sint16);
        virtual void in_up(Sint16);
        virtual void in_down(Sint16);
        virtual void in_sp1(Sint16);
        virtual void in_sp2(Sint16);
        //This should be the same for all vikings
        virtual void in_act(Sint16);
        virtual void in_use(Sint16);
    protected:
        //add places
        virtual void addTouch(std::set<Object *>&);
        virtual void addEnter(std::set<Object *>&);
        //remove places
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        //Items
        void removeItem(Uint8 num);
        Item* moveItem(Uint8 num);
        Object* dropItem(Uint8 num, bool right=true);
        //fall -> not fall, run into wall
        virtual void land();
        //when it dies...
        virtual void die();
        //move
        virtual Hit move(Uint16 dt, bool check=false);
        //Common animations
        Animation* im_left;
        Animation* im_right;
        Animation* im_fall_left;
        Animation* im_fall_right;
        Animation* im_krit_left;
        Animation* im_krit_right;
        Animation* im_land_left;
        Animation* im_land_right;
        //Common audio chunks
        Mix_Chunk*  au_land;
        Mix_Chunk*  au_act;
        Mix_Chunk*  au_newitem;
        Mix_Chunk*  au_hit;
        //Items
        Item* items[MAX_ITEMS];
        Uint8 currentitem;
};

//Monsters
class Monster : public Character {
    public:
        Monster(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Monster");
        virtual ~Monster();
        virtual void removedObject(Object*);
        //updates the current animation state
        virtual void updateAnimState();
        //Define these for each viking...
        virtual void idle(Uint16);
        virtual void fall(Uint16);
        //TODO: add AI commands
    protected:
        //add places
        virtual void addTouch(std::set<Object *>&);
        virtual void addEnter(std::set<Object *>&);
        //remove places
        virtual void removeTouch(std::set<Object *>&);
        virtual void removeEnter(std::set<Object *>&);
        //fall -> not fall, run into wall
        virtual void land();
        //when it dies...
        virtual void die();
        //ai functions
        virtual void runAI(Uint16) { }
        virtual void ai_left(Uint16) { }
        virtual void ai_right(Uint16) { }
        virtual void ai_attack(Viking*, Uint16) { }
        Sint16 Dai,Dattack;
        //Attack targets
        std::set<Viking *> targets;
        //move
        virtual Hit move(Uint16 dt, bool check=false);
        //common animations
        Animation* im_left;
        Animation* im_right;
};


class ObjectsPool {
    public:
        ObjectsPool();
        ~ObjectsPool();
        //add objects
        Object* addObjectbyName(const string& obj, const string& image, Uint16 x=0, Uint16 y=0, const string& arg1="0", const string& arg2="0", const string& arg3="0");
        Object*            addObject(Object* object);
        Character*         addCharacter(Character* newcharacter);
        Viking*            addViking(Viking* newviking);
        Monster*           addMonster(Monster* newmonster);
        //get objects
        Object*            getObject(const string& oname);
        //(re)move objects
        object_iterator    removeObject(object_iterator it);
        object_iterator    removeObject(Object* object);
        Object*            moveObject(Object* object);

        Viking* switchViking();

        //pools
        std::set<Object *>    objectspool;
        std::set<Character *> characterspool;
        std::set<Viking *>    vikingspool;
        std::set<Monster *>   monsterspool;
    private:
        //viking number of the currently selected viking
        viking_iterator currentviking;
        Mix_Chunk* au_switch;
};


//Items
class Item : public Object {
    public:
        Item(string img, Uint16 xpos=0, Uint16 ypos=0, string name="Item");
        virtual ~Item();
        virtual bool act(Object*) { return false; }
        void setOwner(Viking* vik) {
            owner=vik;
        }
    protected:
        Viking* owner;
};

//Background
class Background : public Object {
    public:
        Background(string imagename);
        virtual ~Background();
};


//gfxeng.cpp
class GraphicsEngine {
    public:
        GraphicsEngine();
        ~GraphicsEngine();
        //drawing
        void renderScene(bool insist=true);
        void toggleVikingBar();
    private:
        inline SDL_Rect clipToBG(SDL_Rect dest) const;
        //updates backpos and returns the new shift vector (ignore w,h)
        inline SDL_Rect setShift(SDL_Rect center);
        //simple vector addition, should be replaced by operator
        inline SDL_Rect* shiftMapArea(SDL_Rect& area, const SDL_Rect& shift);
        //draw viking bar
        inline void drawVikingBar();
        //currently visible part of the maparea
        SDL_Rect backpos;
        //main screen
        SDL_Surface* screen;
        //viking bar
        SDL_Rect bar;
        //viking bar
        bool vbar;
        Animation* lifeimage;
        //fps stuff
        Uint16 Dfps,Dframes,currentfps,tcurrent;
};

//sfxeng.cpp
class SoundsEngine {
    public:
        SoundsEngine();
        ~SoundsEngine();
        void playWAV(Mix_Chunk* wav);
        void pauseMusic();
        void resumeMusic();
    private:
        Mix_Music* theme;
        bool disabled;
};


//input.cpp
class InputHandler {
    public:
        InputHandler();
        ~InputHandler();
        //check for keyboard events
        void pollEvents();
        bool getState(Uint16 cstate) {
            return (state&cstate);
        }
        void setState(Uint16 cstate) {
            state|=cstate;
        }
        void unsetState(Uint16 cstate) {
            state&=~cstate;
        }
    private:
        int filterEvents(const SDL_Event *event);
        Uint16 state;
        Mix_Chunk* au_pause;
};

//anim.cpp
class AnimHandler {
    public:
        AnimHandler();
        ~AnimHandler();
        //run animations
        void runAnims();
        Uint16 resetTime();
    private:
        Uint16 tstart, tcurrent, dt;
};


//map.cpp
class Map {
    public:
        Map(string mapname);
        ~Map();
        Hit checkPlace(const SDL_Rect& src, const SDL_Rect& dest) const;
        std::set<Character *> getCharactersIn(const SDL_Rect& rect, bool touch=false, Uint16 radius=0) const;
        std::set<Viking *> getVikingsIn(const SDL_Rect& rect, bool touch=false, Uint16 radius=0) const;
        Uint16 getDirection(const SDL_Rect& src, const SDL_Rect& dest) const;
    private:
        //map loading
        inline int loadMapData(const string& mapname);
};

//cleanup
int quitGame(int);
//bmp/wav loading
SDL_Surface* loadImage(string imagename);
Mix_Chunk* loadWAV(string wavname);
//parse command line arguments and config files
int readConfig(const string& filename);
void parseInput(int argc,char* argv[]);
void usage();
//filter events
int filterEvents(const SDL_Event *event);
string itos(int);

extern GraphicsEngine* gfxeng;
extern SoundsEngine* sfxeng;
extern ObjectsPool* pool;
extern Viking* viking;
extern Background* background;
extern Map* map;
extern InputHandler* input;
extern AnimHandler* anim;
extern Font* font;
extern Font* font2;
extern bool paused;
extern bool failed;
extern bool fps;
