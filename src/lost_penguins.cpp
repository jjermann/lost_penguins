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
#include "editor.h"
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
    font=new Font(imgcache->loadImage(1,"font_arial_white_16_01.png").surface);
    font2=new Font(imgcache->loadImage(1,"font_arial_red_16_01.png").surface);
    cout << "InputHandler...\n";
    input=new InputHandler();
    cout << "Initializing Scenario...\n";
    scenario=new Scenario();

    startScreen();

    while (true) {
        input->update();
        scenario->physic->update();
        gfxeng->draw();
        SDL_Delay(1);
    }

    quitGame(-2);
}

void startScreen() {
    scenario->resetScenario();
    gfxeng->setMenuBG();
    setMenu(new StartMenu());
    setGameMode(GAME_MENU);
}

int quitGame(int errorcode=0) {
    cout << endl << "Quitting game (exit code: " << errorcode << ")...\n";
    closeMenus();
    if (editor) {
        cout << "Deleting Editor...\n";
        delete editor;
    }
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
    config.full=false;
    config.audio_rate = MIX_DEFAULT_FREQUENCY;
    config.audio_format = MIX_DEFAULT_FORMAT;
    config.audio_channels = 2;
    config.datadir="data/";
    config.anim_file="animation_data.anim";
    config.map="";
    config.scenario="lv1.sce";
    config.lvlscale=2;
    config.onlymap=false;

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
    config.keybind[KEY_DEBUG]   = SDLK_F4;
    config.keybind[KEY_NOANIM]  = SDLK_F3;
    config.keybind[KEY_FPS]     = SDLK_F2;
    config.keybind[KEY_BAR]     = SDLK_F1;
    config.keybind[KEY_FULL]    = SDLK_f;
    config.keybind[KEY_QUIT]    = SDLK_q;
    config.keybind[KEY_ADD_SEL] = SDLK_LSHIFT;
    config.keybind[KEY_RM_SEL]  = SDLK_LCTRL;
    config.keybind[KEY_DEL]     = SDLK_DELETE;

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
            if (config.datadir.substr(config.datadir.size() - 1, 1)!="/") config.datadir+="/";
        } else if (option=="animfile") {
            config.anim_file=arg1;
        } else if (option=="width") {
            config.width=atoi(arg1.c_str());
        } else if (option=="height") {
            config.height=atoi(arg1.c_str());
        } else if (option=="full") {
            if (arg1=="1") config.full=true;
            else config.full=false;
        } else if (option=="map") {
            config.map=arg1;
            config.onlymap=true;
        } else if (option=="scenario") {
            config.scenario=arg1;
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
        } else if ( strcmp(argv[i], "-full") == 0 || strcmp(argv[i], "-fs") == 0) {
            config.full=SDL_FULLSCREEN;
        } else if ( strcmp(argv[i], "-map") == 0 ) {
            config.map=argv[i+1];
            config.map+=".cfg";
            config.onlymap=true;
            i++;
        } else if ( strcmp(argv[i], "-scenario") == 0 ) {
            config.scenario=argv[i+1];
            config.scenario+=".sce";
            i++;
        } else if ( strcmp(argv[i], "-datadir") == 0 ) {
            config.datadir=argv[i+1];
            if (config.datadir.substr(config.datadir.size() - 1, 1)!="/") config.datadir+="/";
            i++;
        } else if ( strcmp(argv[i], "-animfile") == 0 ) {
            config.anim_file=argv[i+1];
            i++;
        } else if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage();
        } else usage();
    }
}

void usage() {
    cout << "Usage: lost_penguins [OPTIONS]\n\n";
    cout << "  -datadir     Changes the data directory.            Default: data/\n";
    cout << "  -animfile    Changes the animation file name.       Default: animation_data.anim\n";
    cout << "  -w, -width   Changes resolution (width) of game.    Default: 640\n";
    cout << "  -h, -height  Changes resolution (height) of game.   Default: 480\n";
    cout << "  -fs, -full   Enable fullscreen.                     Default: disabled\n";
    cout << "  -map         Load specified map from data dir.      Default: disabled (map1 as a fallback)\n";
    cout << "  -scenario    Load specified Scenario from data dir. Default: lv1\n";
    cout << "  -h, --help   Show this text \n";
    quitGame(4);
}
