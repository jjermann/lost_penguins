#ifndef DEF_COMMON_H
#define DEF_COMMON_H 1

/// Common header file
///
/// \todo Create a game class and move almost all remaining global
///   variables/functions there...
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
#include <boost/shared_ptr.hpp>

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
class PhysicHandler;
class Font;
class Event;
class Weapon;
class EmptyAnimation;
class Animation;
class Menu;
class Box;
class Editor;

typedef std::set<Object *>::iterator object_iterator;
typedef std::set<Character *>::iterator character_iterator;
typedef std::set<Player *>::iterator player_iterator;
typedef std::set<Monster *>::iterator monster_iterator;
typedef std::set<Object *>::reverse_iterator object_riterator;
typedef std::set<Character *>::reverse_iterator character_riterator;
typedef boost::shared_ptr<EmptyAnimation> EmptyAnimationPtr;
typedef std::map<string,string> ParameterMap;

//Data
#define DATA_LVLFPS 6
#define DATA_LVLDUR 1000
#define DATA_LVLPLAYER_W 25
#define DATA_LVLPLAYER_H 30

//General definitions
#define NOTHING                 0x00000000
#define ALL                     0xFFFFFFFF
#define DIR_RIGHT               0x00000001
#define DIR_LEFT                0x00000002
#define DIR_UP                  0x00000004
#define DIR_DOWN                0x00000008
#define DIR_ALL                 0x0000000F
#define DIR_UPR                 0x00000005
#define DIR_UPL                 0x00000006
#define DIR_DWR                 0x00000009
#define DIR_DWL                 0x0000000A
#define DIR_LR                  0x00000003

//Game states
#define GAME_PAUSED             0x00000001
#define GAME_PLAY               0x00000002
#define GAME_MENU               0x00000004
#define GAME_EDIT               0x00000008
#define GAME_EDIT_NOANIM        0x00000010
#define GAME_TEXT_INPUT         0x00000020

//Animation types
#define ATYPE_ONCE              0x00000001
#define ATYPE_LOOP              0x00000002
#define ATYPE_SWING             0x00000004
#define ATYPE_STEP              0x00000008
#define ATYPE_ONCE_REV          0x00000010
#define ATYPE_LOOP_REV          0x00000020
#define ATYPE_SWING_REV         0x00000040
#define ATYPE_STEP_REV          0x00000080
#define ATYPE_ALL_ONCE          0x00000011
#define ATYPE_ALL_LOOP          0x00000022
#define ATYPE_ALL_SWING         0x00000044
#define ATYPE_ALL_NORMAL        0x0000000F
#define ATYPE_ALL_REV           0x000000F0
#define ATYPE_ST_SWITCH         0x00000100
#define ATYPE_ONCE_S            0x00000101
#define ATYPE_ONCE_S_REV        0x00000110

//Description formats
#define DESC_NONE               0x00000000
#define DESC_LVLANIM            0x00000001
#define DESC_ANIM_LVLANIM       0x00000002     

enum ConfigKey {
    KEY_START,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_UP,
    KEY_DOWN,
    KEY_SP1,
    KEY_SP2,
    KEY_ACT,
    KEY_USE,
    KEY_DROP,
    KEY_SWITCH,
    KEY_PAUSE,
    KEY_MENU,
    KEY_FPS,
    KEY_BAR,
    KEY_FULL,
    KEY_QUIT,
    KEY_NOANIM,
    KEY_DEBUG,
    KEY_ADD_SEL,
    KEY_RM_SEL,
    KEY_DEL
};

enum BasePointType {
    BP_LU,
    BP_LM,
    BP_LD,
    BP_MU,
    BP_MM,
    BP_MD,
    BP_RU,
    BP_RM,
    BP_RD
};

enum AllignType {
    AT_LU,
    AT_LM,
    AT_LD,
    AT_MU,
    AT_MM,
    AT_MD,
    AT_RU,
    AT_RM,
    AT_RD
};

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
    bool full;
    Uint16 audio_format;
    int audio_rate;
    int audio_channels;
    string datadir;
    string map;
    string scenario;
    string anim_file;
    SDLKey keybind[30];
    double lvlscale;
    bool onlymap;
};

/**\brief Frame format

*/
struct Frame {
        Frame(SDL_Surface*,SDL_Rect);
        Frame();
        SDL_Surface* image;
        //Frame position (Source Rectangle)
        SDL_Rect    pos;
};

/**\brief Image structure

*/
struct Image {
    SDL_Surface* surface;
    std::vector<SDL_Rect> description;
};

//global functions
//@{
/// Resets everything and shows the start screen again
void startScreen();
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
/// Set parameters from files
ParameterMap getFileParameters(const string& filelist);
/// Parse a string parameter list and return it as a map<string,string>
ParameterMap getParameters(const string& parameterlist, char delim=',', char delimsub='=');
/// Convert the parameter map to a string
string putParameters(const ParameterMap& parameters, char delim=',', char delimsub='=');
/// Returns true if the parameter was found
bool hasParam(const ParameterMap& parameters, const string& str);
/// Returns the direction from the given string: simply include one or a
/// combination of "up", "down", "left", "right" in the string
Uint16 getDirFromString(const string& str);

/// Parse the command line options
void parseInput(int argc,char* argv[]);
/// Print the usage
void usage();
/// Helper function that converts an integer to a string
string itos(int);
/// Helper function that increases/decreases the absolute value
int addAbsolute(int,int);
/// Calculate the fps as frames*1000/duration_in_ms
double calcFPS(Uint16 frames, Uint32 duration);

/// Set game mode
void setGameMode(Uint8);
/// Add game mode
void addGameMode(Uint8);
/// Remove game mode
void removeGameMode(Uint8);
/// Set the current menu
Menu* setMenu(Menu* newmenu);
/// Close the current menus
Menu* closeMenu();
/// Close all menus (menu=NULL)
void closeMenus();

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
/// Game state
extern Uint8 game_mode;
/// Currently used menu, NULL if not inside a menu
extern Menu* menu;
/// Editor
extern Editor* editor;
//@}

#endif
