#ifndef _COMMON_H
#define _COMMON_H 1

/// Common header file
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
class Scenario;
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

/**\brief Collision type

*/
struct Hit {
   Uint16 enter;
   Uint16 touch;
};

/**\brief Game configuration structure

*/
struct Config {
    Uint16 width;
    Uint16 height;
    Uint8 bpp;
    bool full;
    Uint16 audio_format;
    int audio_rate;
    int audio_channels;
    string datadir;
    string map;
};

/**\brief Frame format

*/
struct Frame {
    SDL_Surface* image;
    //Frame position (Source Rectangle)
    SDL_Rect    pos;
};

//global functions
//@{
/// Quits the game with a error code
/// \return Error code, 0 if successfull
int quitGame(int);

/// \brief Parse the configuration file
///
/// Sets the default values, reads and parses the configuration file
/// afterwards
/// \return 1 if the file was found, 0 otherwise
/// \todo Create a better config format
int readConfig(const string& filename);

/// Parse the command line options
void parseInput(int argc,char* argv[]);
/// Print the usage
void usage();
/// Helper function that converts an integer to a string
string itos(int);
/// Helper function boost that increases/decreases the absolute value
int boost(int,int);
//@}

//global variables
//@{
/// Game configuration
extern Config config;
/// Image Cache
extern ImageCache* imgcache;
/// Sound Cache
extern SoundCache* sndcache;
/// Graphics Engine
extern GraphicsEngine* gfxeng;
/// Sounds Engine
extern SoundsEngine* sfxeng;
/// Current map
extern Scenario* scenario;
/// Input Handler
extern InputHandler* input;
/// Font1
extern Font* font;
/// Font2
extern Font* font2;
/// True if the game is paused
extern bool paused;
//@}

#endif
