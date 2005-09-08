#include "common.h"
#include "gfxeng.h"
#include "editor.h"

ImageCache* imgcache;
SoundCache* sndcache;
GraphicsEngine* gfxeng;
SoundsEngine* sfxeng;
Scenario* scenario;
InputHandler* input;
Font* font;
Font* font2;
Config config;
Uint8 game_mode=NOTHING;
Menu* menu=NULL;
Box* box=NULL;
Editor* editor=NULL;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
const Uint32 rmask=0xff000000;
const Uint32 gmask=0x00ff0000;
const Uint32 bmask=0x0000ff00;
const Uint32 amask=0x000000ff;
#else
const Uint32 rmask=0x000000ff;
const Uint32 gmask=0x0000ff00;
const Uint32 bmask=0x00ff0000;
const Uint32 amask=0xff000000;
#endif
Uint32 vflags=SDL_HWSURFACE|SDL_RESIZABLE|SDL_DOUBLEBUF|SDL_HWACCEL;

string itos(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}

ParameterMap getFileParameters(const string& filelist) {
    istringstream filelist_s(filelist);
    ifstream file;
    string filename,parameterlist;
    ParameterMap file_parameters, line_parameters;
    ParameterMap::iterator it;

    while (std::getline(filelist_s,filename,':')) {
        file.open((config.datadir+filename).c_str());
        if (!file) continue;
        while (getline(file,parameterlist)) {
            if (parameterlist.empty()) continue;
            if (parameterlist[0]=='#') continue;
            line_parameters=getParameters(parameterlist);
            it=line_parameters.begin();
            while (it!=line_parameters.end()) {
                file_parameters[(*it).first]=(*it).second;
                ++it;
            }
        }
        file.close();
        file.clear();
    }
    return file_parameters;
}

ParameterMap getParameters(const string& parameterlist, char delim, char delimsub) {
    ParameterMap parameters;
    istringstream parameterlist_s(parameterlist);
    string parameter,name,value,tmpstr;

    while (std::getline(parameterlist_s,parameter,delim)) {
        istringstream parameter_s(parameter);
        std::getline(parameter_s,name,delimsub);
        std::getline(parameter_s,value);

        /* get rid of leading and trailing spaces: this part is silly ;)
           all spaces, tabs, etc are converted to one single space */
        istringstream name_s(name);
        name="";
        while (name_s >> tmpstr) {
            if (!name.empty()) name+=" ";
            name+=tmpstr;
        }
        istringstream value_s(value);
        value="";
        while (value_s >> tmpstr) {
            if (!value.empty()) value+=" ";
            value+=tmpstr;
        }
        parameters[name]=value;
    }
    return parameters;
}

string putParameters(const ParameterMap& parameters, char delim, char delimsub) {
    string parameterlist;
    ParameterMap::const_iterator it=parameters.begin();
    while (it!=parameters.end()) {
        if (!parameterlist.empty()) {
            parameterlist+=delim;
            parameterlist+=" ";
        }
        parameterlist+=(*it).first;
        parameterlist+=delimsub;
        parameterlist+=(*it).second;
        ++it;
    }
    return parameterlist;
}

bool hasParam(const ParameterMap& parameters, const string& str) {
    if (parameters.find(str)!=parameters.end()) return true;
    else return false;
}

Uint16 getDirFromString(const string& str) {
    Uint16 direction=NOTHING;
    if (str.find("up")    !=string::npos) direction|=DIR_UP;
    if (str.find("down")  !=string::npos) direction|=DIR_DOWN;
    if (str.find("left")  !=string::npos) direction|=DIR_LEFT;
    if (str.find("right") !=string::npos) direction|=DIR_RIGHT;
    return direction;
}

int addAbsolute(int a, int b) {
    if (a>=0) return a=max(0,a+b);
    else return a=min(0,a-b);
}

double calcFPS(Uint16 frames, Uint32 duration) { 
    return frames*1000.0/duration;
}

void setGameMode(Uint8 newmode) {
    game_mode=newmode;
    if (game_mode&GAME_EDIT && !(game_mode&GAME_MENU)) {
        gfxeng->setShowDebug();
        SDL_ShowCursor(SDL_ENABLE);
        gfxeng->resetShift();
    } else {
        gfxeng->unsetShowDebug();
        SDL_ShowCursor(SDL_DISABLE);
    }
    if (!editor) editor=new Editor();
#ifdef DEBUG
    gfxeng->setShowDebug();
#endif
}

void addGameMode(Uint8 addmode) {
    game_mode|=addmode;
    setGameMode(game_mode);
}

void removeGameMode(Uint8 rmmode) {
    game_mode&=~rmmode;
    setGameMode(game_mode);
}

Frame::Frame(SDL_Surface* surface,SDL_Rect rect):
  image(surface),
  pos(rect) { }

Frame::Frame():
  image(NULL) {
    pos.x=pos.y=pos.w=pos.h=0;
}
