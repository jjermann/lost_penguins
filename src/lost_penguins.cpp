#include "common.h"
#include "events.h"
#include "physics.h"
#include "input.h"
#include "font.h"
#include "scenario.h"
#include "imgcache.h"
#include "sndcache.h"
#include "gfxeng.h"
#include "sfxeng.h"
#include "objectpools.h"
#include "menu.h"
#include "players_common.h"


int main(int argc, char* argv[]) {
    cout << "Starting Lost Penguins!\n";
    //Clean exit on Windows close...
    atexit(SDL_Quit);

    cout << "Loading configuration file...\n";
    //Maybe: add some more location checks...
    if (readConfig("/usr/local/etc/lost_penguins.conf")) cout << "Loaded /usr/local/etc/lost_penguins.conf...\n";
    else cout << "Configuration files not found! Set default parameters...\n";

    parseInput(argc,argv);

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_AUDIO)==0) {
        cout << "Initialized SDL...\n";
    } else {
        cout << "Couldn't initialize SDL!\n";
        exit(-1);
    }
    SDL_ShowCursor(SDL_DISABLE);
    //Change directory to datadir

    cout << "ImageCache...\n";
    imgcache=new ImageCache();
    cout << "SoundCache...\n";
    sndcache=new SoundCache();
    cout << "GraphicsEngine...\n";
    gfxeng=new GraphicsEngine();
    cout << "SoundEngine...\n";
    sfxeng=new SoundsEngine();
    cout << "Fonts...\n";
    font=new Font(imgcache->loadImage("font_arial_white_16_01.png"));
    font2=new Font(imgcache->loadImage("font_arial_red_16_01.png"));
    cout << "InputHandler...\n";
    input=new InputHandler();
    cout << "Initializing Scenario...\n";
    scenario=new Scenario();

    gfxeng->setMenuBG();
    menu=NULL;
    setMenu(new StartMenu());

    while (true) {
        input->update();
        if (game_mode&GAME_PLAY) scenario->physic->update();
        gfxeng->draw();
    }

    quitGame(-2);
}

int quitGame(int errorcode=0) {
    cout << endl << "Quitting game (exit code: " << errorcode << ")...\n";
    closeMenus();
    delete scenario;
    cout << "Scenario closed...\n";
    delete sfxeng;
    delete gfxeng;
    cout << "GraphicsEngine deleted...\n";
    delete input;
    delete font;
    cout << "Deleting SoundCache...\n";
    delete sndcache;
    cout << "Deleting ImageCache...\n";
    delete imgcache;
    cout << "Quiting SDL...\n";
    
    SDL_Quit();
    exit(errorcode);
}

int readConfig(const string& filename) {
    ifstream configfile;
    string tmpline, option, arg1;
    
    //default values
    config.width=640;
    config.height=480;
    config.bpp=16;
    config.full=false;
    config.audio_rate = MIX_DEFAULT_FREQUENCY;
    config.audio_format = MIX_DEFAULT_FORMAT;
    config.audio_channels = 2;
    config.datadir="data/";
    config.map="map1.cfg";

    //key bindings
    config.keybind[KEY_LEFT]    = SDLK_LEFT;
    config.keybind[KEY_RIGHT]   = SDLK_RIGHT;
    config.keybind[KEY_UP]      = SDLK_UP;
    config.keybind[KEY_DOWN]    = SDLK_DOWN;
    config.keybind[KEY_SP1]     = SDLK_SPACE;
    config.keybind[KEY_SP2]     = SDLK_LSHIFT;
    config.keybind[KEY_ACT]     = SDLK_RETURN;
    config.keybind[KEY_USE]     = SDLK_INSERT;
    config.keybind[KEY_DROP]    = SDLK_DELETE;
    config.keybind[KEY_SWITCH]  = SDLK_LCTRL;
    config.keybind[KEY_PAUSE]   = SDLK_TAB;
    config.keybind[KEY_MENU]    = SDLK_ESCAPE;
    config.keybind[KEY_FPS]     = SDLK_F2;
    config.keybind[KEY_BAR]     = SDLK_F1;
    config.keybind[KEY_FULL]    = SDLK_f;
    config.keybind[KEY_QUIT]    = SDLK_q;

    configfile.open(filename.c_str());
    if (!configfile) return 0;

    //check every line
    while (getline(configfile,tmpline)) {
        std::istringstream tmpstream(tmpline);
        tmpstream >> option >> arg1;

        //Skip empty lines
        if (option.empty()) continue;
        //Skip comments
        if (option[0]=='#') continue;
        if (option=="datadir") {
            config.datadir=arg1;
        } else if (option=="width") {
            config.width=atoi(arg1.c_str());
        } else if (option=="height") {
            config.height=atoi(arg1.c_str());
        } else if (option=="bpp") {
            config.bpp=atoi(arg1.c_str());
        } else if (option=="full" && arg1=="1") {
            config.full=true;
        } else if (option=="map") {
            config.map=arg1;
        //TODO: add keybindings
        } else {
            cout << "Unknown option: " << option << endl;
        }
    }

    return 1;
}

void parseInput(int, char* argv[]) {
    for(Uint8 i=1; argv[i]; i++) {
        if ( strcmp(argv[i], "-width") == 0 || strcmp(argv[i], "-w") == 0) {
            config.width=atoi(argv[i+1]);
            i++;
        } else if ( strcmp(argv[i], "-height") == 0 || strcmp(argv[i], "-h") == 0) {
            config.height=atoi(argv[i+1]);
            i++;
        } else if ( strcmp(argv[i], "-bpp") == 0 ) {
            config.bpp=atoi(argv[i+1]);
            i++;
        } else if ( strcmp(argv[i], "-full") == 0 || strcmp(argv[i], "-fs") == 0) {
            config.full=SDL_FULLSCREEN;
        } else if ( strcmp(argv[i], "-map") == 0 ) {
            config.map=argv[i+1];
            config.map+=".cfg";
            i++;
        } else if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage();
        } else usage();
    }
}

void usage() {
    cout << "Usage: lost_penguins [OPTIONS]\n\n";
    cout << "  -w, -width   Changes resolution (width) of game.    Default: 640\n";
    cout << "  -h, -height  Changes resolution (height) of game.   Default: 480\n";
    cout << "  -bpp         Specifies color depth (bpp) of screen. Default: 16\n";
    cout << "  -fs, -full   Enable fullscreen.                     Default: disabled\n";
    cout << "  -map         Load specified map from data dir.      Default: map1\n";
    cout << "  -h, --help   Show this text \n";
    quitGame(4);
}
