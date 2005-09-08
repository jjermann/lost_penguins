#include "common.h"
#include "objectpools.h"
#include "players_common.h"
#include "imgcache.h"
#include "sndcache.h"
#include "sfxeng.h"
#include "physics.h"
#include "scenario.h"


Scenario::Scenario():
  background(NULL),
  area(NULL),
  player(NULL),
  failed(false),
  finnished(false),
  mapname(""),
  scenarioname(""),
  bgimage(""),
  currentmap(0) {
    cout << "Scenario: ImageCache...\n";
    imgcache=new ImageCache();
    cout << "Scenario: SoundCache...\n";
    sndcache=new SoundCache();
    cout << "Scenario: PhysicHandler...\n"; 
    physic=new PhysicHandler();
    cout << "Map: ObjectsPool...\n";
    pool=new ObjectsPool();
    max_obj_num=0;
}

Scenario::~Scenario() {
    cout << endl << "Closing map...\n";
    delete pool;
    cout << "Map: Deleted Pools...\n";
    if (background) delete background;
    cout << "Map: Deleted Background...\n";
    delete physic;
    cout << "Scenario: Deleted PhysicHandler...\n";
    delete sndcache;
    cout << "Scenario: Deleted SoundCache...\n";
    delete imgcache;
    cout << "Scenario: Deleted ImageCache... (Scenario closed)\n";
}

inline void Scenario::reinitMap() {
    if (pool && !(pool->empty())) {
        delete pool;
        pool=new ObjectsPool();
    } else if (!pool) pool=new ObjectsPool();
    if (background) delete background;
    if (physic) delete physic;
    physic=new PhysicHandler();
    sfxeng->stopMusic();
    background=NULL;
    area=NULL;
    player=NULL;
    max_obj_num=0;
    finnished=false;
    failed=false;
}

void Scenario::newMap(const ParameterMap& bg_parameters) {
    mapbuf.clear();
    mapbuf.push_back("Background "+putParameters(bg_parameters));
    mapbuf.push_back("#ENDHEADER");
}
int Scenario::loadMapBuf(string mapn) {
    mapname=mapn;
    ifstream mapfile;
    string tmpline;
    string loadfile=config.datadir+mapname;

    mapfile.open(loadfile.c_str());
    if (mapfile) {
        cout << "Loading new map: " << loadfile << endl;
    } else {
        cout << "Map loading failed: " << loadfile << " not found!\n";
        return 2;
    }

    mapbuf.clear();
    while (getline(mapfile,tmpline)) {
        mapbuf.push_back(tmpline);
    }

    mapfile.close();
    mapfile.clear();
    return 0;
}

int Scenario::reloadMap() {
    reinitMap();
    string cname;
    Uint16 x,y;
    ParameterMap parameters;
    bool header=true;

    //check every line
    for (Uint16 linenum=0; linenum<mapbuf.size(); linenum++) {
        //parse the header first, end it with #ENDHEADER!
        if (header) {
            std::istringstream tmpstream(mapbuf[linenum]);
            tmpstream >> cname;
            if (cname[0]=='#') {
                if (cname=="#ENDHEADER") header=false;
                continue;
            } else {
                string parameterlist((istreambuf_iterator<char>(tmpstream)), istreambuf_iterator<char>());
                parameters=getParameters(parameterlist);
                if (cname=="Background" && (!background)) {
                    background=new Background(parameters);
                    if (background) {
                        area=background->getPos();
                        bgimage=parameters["image"];
                    }
                }
            }
        // Background has to be loaded...
        } else if (background) {
            cname.erase();
            x=y=0;
            std::istringstream tmpstream(mapbuf[linenum]);
            tmpstream >> cname >> x >> y;
            //Skip empty lines
            if (cname.empty()) continue;
            if (cname[0]=='#') continue;

            string parameterlist((istreambuf_iterator<char>(tmpstream)), istreambuf_iterator<char>());
            parameters=getParameters(parameterlist);
            pool->addObjectbyName(cname,x,y,parameters);
        } else {
            cout << "Map loading failed: No background found!" << endl;
            return 1;
        }
    }

    sfxeng->playMusic((config.datadir + "01theme.wav").c_str());
    if (pool->playerspool.size()>0) player=*pool->playerspool.begin();
    else player=NULL;
    if (player==NULL) cout << "No player found!\n";
    return 0;
}

int Scenario::loadMap(string mapn) {
    if (loadMapBuf(mapn)==0 && reloadMap()==0) return 0;
    else return 1;
}

int Scenario::loadMap(Uint8 level) {
    if (level<maps.size()) return loadMap(maps[level]+".cfg");
    else return 3;
}

void Scenario::loadNextMap() {
    if (failed) {
        cout << "Reloading current map" << endl;
        if (reloadMap()!=0) {
            cout << "Reloading failed!" << endl;
        }
    } else if ((++currentmap)<maps.size()) {
        cout << "Loading next map" << endl;
        if (loadMap(maps[currentmap]+".cfg")!=0) {
            cout << "Map loading failed!" << endl;
        }
    } else {
        winScenario();
    }
}

int Scenario::startScenario() {
    currentmap=0;
    return loadMap();
}

void Scenario::winScenario() {
    cout << "You won the scenario!" << endl;
    startScreen();
}

void Scenario::resetScenario() {
    reinitMap();
    currentmap=0;
    maps.clear();
    playlist.clear();
}

int Scenario::loadScenario(string scenarion) {
    reinitMap();
    maps.clear();
    currentmap=0;

    scenarioname=scenarion;
    ifstream scenariofile;
    string tmpline;
    string loadfile=config.datadir+scenarioname;

    scenariofile.open(loadfile.c_str());
    if (scenariofile) {
        cout << "Loading new scenario file: " << loadfile << endl;
    } else {
        cout << "Scenario file loading failed: " << loadfile << " not found!\n";
        return 2;
    }

    string mname, arg1, arg2;
    bool header=false;

    //check every line
    while (getline(scenariofile,tmpline)) {
        //parse the header, always start a header with #HEADER and end it with #ENDHEADER!
        if (header) {
            mname.erase();
            arg1.erase();
            arg2.erase();
            std::istringstream tmpstream(tmpline);
            tmpstream >> mname >> arg1 >> arg2;
            if (mname[0]=='#') {
                if (mname=="#ENDHEADER") header=false;
                continue;
            }
            continue;
        }
        mname.erase();
        arg1=arg2="";
        std::istringstream tmpstream(tmpline);
        tmpstream >> mname >> arg1 >> arg2;

        //Skip empty lines
        if (mname.empty()) continue;
        //Skip comments
        if (mname[0]=='#') {
            if (mname=="#HEADER") header=true;
            continue;
        }

        maps.push_back(mname);
    }

    scenariofile.close();
    scenariofile.clear();

    if (maps.size()>0) return 0;
    else return 1;
}

Uint16 Scenario::getDirection(const SDL_Rect& src, const SDL_Rect& dest) const {
    Uint16 dir=NOTHING;
    if ((src.x+src.w) < (dest.x+dest.w)) dir|=DIR_RIGHT;
    if (src.x > dest.x)                  dir|=DIR_LEFT;
    if ((src.y+src.h) < (dest.y+dest.h)) dir|=DIR_DOWN;
    if (src.y > dest.y)                  dir|=DIR_UP;
    return dir;
}

Hit Scenario::checkPlace(const SDL_Rect& src, const SDL_Rect& dest) const {
    Hit hit;
    hit.enter=NOTHING;
    hit.touch=NOTHING;
    if (src.x < dest.x) hit.enter|=DIR_LEFT;
    if (src.x==dest.x) hit.touch|=DIR_LEFT;
    if (src.y < dest.y) hit.enter|=DIR_UP;
    if (src.y==dest.y) hit.touch|=DIR_UP;
    if ((src.x+src.w) > (dest.x+dest.w)) hit.enter|=DIR_RIGHT;
    if ((src.x+src.w)==(dest.x+dest.w)) hit.touch|=DIR_RIGHT;
    if ((src.y+src.h) > (dest.y+dest.h)) hit.enter|=DIR_DOWN;
    if ((src.y+src.h)==(dest.y+dest.h)) hit.touch|=DIR_DOWN;
    return hit;
}


std::set<Character *> Scenario::getCharactersIn(Uint16 mask, const SDL_Rect& rect, bool touch, Uint16 distance, Uint16 dir) const {
    SDL_Rect tmprect=rect;
    if (dir&DIR_LEFT) {
        tmprect.x-=distance;
        tmprect.w+=distance;
    }
    if (dir&DIR_RIGHT) {
        tmprect.x+=distance;
        tmprect.w+=distance;
    }
    if (dir&DIR_UP) {
        tmprect.y-=distance;
        tmprect.h+=distance;
    }
    if (dir&DIR_DOWN) {
        tmprect.y+=distance;
        tmprect.h+=distance;
    }
    std::set<Character *> tmpset;
    character_iterator cit=pool->characterspool.begin();
    while (cit != pool->characterspool.end()) {
        if ((((*cit)->getType())&mask) && ((*cit)->isIn(tmprect,touch))) tmpset.insert(*cit);
        ++cit;
    }
    return tmpset;
}
