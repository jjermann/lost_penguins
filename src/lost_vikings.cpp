#include "common.h"
#include "events.h"
#include "anim.h"
#include "input.h"
#include "font.h"
#include "map.h"
#include "imgcache.h"
#include "sndcache.h"
#include "gfxeng.h"
#include "sfxeng.h"
#include "objectpools.h"
#include "vikings_common.h"

using namespace std;

int main(int argc, char* argv[]) {
    cout << "Starting Lost Vikings!\n";
    //Clean exit on Windows close...
    atexit(SDL_Quit);

    cout << "Loading configuration file...\n";
    //Maybe: add some more location checks...
    if (readConfig("/usr/local/etc/lost_vikings.conf")) cout << "Loaded /usr/local/etc/lost_vikings.conf...\n";
    else cout << "Configuration files not found! Set default parameters...\n";

    parseInput(argc,argv);

    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_AUDIO)==0) {
        cout << "Initialized SDL...\n";
    } else {
        cout << "Couldn't initialize SDL!\n";
        exit(-1);
    }
    SDL_ShowCursor(SDL_DISABLE);
    SDL_SetEventFilter(filterEvents);

    maparea=NULL;
    cout << "ImageCache...\n";
    imgcache=new ImageCache();
    cout << "SoundCache...\n";
    sndcache=new SoundCache();
    cout << "GraphicsEngine...\n";
    gfxeng=new GraphicsEngine();
    cout << "Fonts...\n";
    font=new Font(imgcache->loadImage("font_arial_white_16_01.png"));
    font2=new Font(imgcache->loadImage("font_arial_12_01.bmp"));
    cout << "SoundEngine...\n";
    sfxeng=new SoundsEngine();
    cout << "ObjectsPool and Map...\n";
    pool=new ObjectsPool();
    curmap=new Map(config.map);
    cout << "InputHandler...\n";
    input=new InputHandler();
    cout << "AnimHandler...\n";
    anim=new AnimHandler();

    cout << "Starting game...\n" << endl;
    while (true) {
        //Drawing
        gfxeng->renderScene();
        //Check input
        input->pollEvents();
        //Run Animations
        anim->runAnims();
    }

    quitGame(-2);
}

//Clean quit with optional errorcode (default 0)
int quitGame(int errorcode=0) {
    cout << endl << "Quitting game (exit code: " << errorcode << ")...\n";
    delete curmap;
    delete pool;
    cout << "Pools deleted...\n";
    delete sfxeng;
    delete gfxeng;
    cout << "GraphicsEngine deleted...\n";
    delete input;
    delete anim;
    delete font;
    delete background;
    cout << "Deleting SoundCache...\n";
    delete sndcache;
    cout << "Deleting ImageCache...\n";
    delete imgcache;
    cout << "Quiting SDL...\n";
    
    SDL_Quit();
    exit(errorcode);
}

//PRE:  file name of configuration file
//ACT:  sets default values and reads the configuration file
//POST: 1 if the file was found 0 otherwise
//TODO: better config format
int readConfig(const string& filename) {
    ifstream configfile;
    string tmpline, option, arg1;
    
    //default values
    config.width=640;
    config.height=480;
    config.full=0;
    config.audio_rate = MIX_DEFAULT_FREQUENCY;
    config.audio_format = MIX_DEFAULT_FORMAT;
    config.audio_channels = 2;
    config.datadir="data/";
    config.map="map1.cfg";

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
            cout << arg1 << endl;
        } else if (option=="width") {
            config.width=atoi(arg1.c_str());
        } else if (option=="height") {
            config.height=atoi(arg1.c_str());
        } else if (option=="full" && arg1=="1") {
            config.full=SDL_FULLSCREEN;
        } else if (option=="map") {
            config.map=arg1;
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
            i++;
        } else if ( strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage();
        } else usage();
    }
}

void usage() {
    cout << "Usage: lost_vikings [OPTIONS]\n";
    cout << "  -w, -width	Changes resolution (width) of game.  Default: 640\n";
    cout << "  -h, -height	Changes resolution (height) of game. Default: 480\n";
    cout << "  -fs, -full	Enable fullscreen.                   Default: disabled\n";
    cout << "  -map		Load specified map from data dir.    Default: map1\n";
    cout << "  -h, --help	Show this text \n";
    quitGame(4);
}
