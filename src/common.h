#ifndef _COMMON_H
#define _COMMON_H 1

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <math.h>
#include <string>
#include <dlfcn.h>
#include <SDL.h>
#include <SDL_mixer.h>
#ifdef SDL_IMAGE
#include <SDL_image.h>
#endif

using std::string;
using namespace std;

class Object;
class Character;
class Player;
class Monster;
class ImageCache;
class SoundCache;
class GraphicsEngine;
class SoundsEngine;
class ObjectsPool;
class Background;
class Map;
class InputHandler;
class AnimHandler;
class Font;
class Event;
class Weapon;
class Animation;

typedef std::set<Object *>::iterator object_iterator;
typedef std::set<Character *>::iterator character_iterator;
typedef std::set<Player *>::iterator player_iterator;
typedef std::set<Monster *>::iterator monster_iterator;

//General definitions
#define NOTHING         0x00000000
#define ALL             0xFFFFFFFF
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

struct Hit {
   Uint16 enter;
   Uint16 touch;
};

struct Config {
    Uint16 width;
    Uint16 height;
    bool full;
    Uint16 audio_format;
    int audio_rate;
    int audio_channels;
    string datadir;
    string map;
};

struct Frame {
    SDL_Surface* image;
    //Frame position (Source Rectangle)
    SDL_Rect    pos;
};

//global functions
int quitGame(int);
int readConfig(const string& filename);
void parseInput(int argc,char* argv[]);
void usage();
string itos(int);

//global variables
extern Config config;
extern SDL_Rect* maparea;
extern ImageCache* imgcache;
extern SoundCache* sndcache;
extern GraphicsEngine* gfxeng;
extern SoundsEngine* sfxeng;
extern ObjectsPool* pool;
extern Player* player;
extern Background* background;
extern Map* curmap;
extern InputHandler* input;
extern AnimHandler* anim;
extern Font* font;
extern Font* font2;
extern bool paused;
extern bool failed;
extern bool fps;

#endif
