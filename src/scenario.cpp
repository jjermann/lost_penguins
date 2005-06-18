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
  name("") {
    cout << "Scenario: ImageCache...\n";
    imgcache=new ImageCache();
    cout << "Scenario: SoundCache...\n";
    sndcache=new SoundCache();
    cout << "Scenario: PhysicHandler...\n"; 
    physic=new PhysicHandler();
    cout << "Map: ObjectsPool...\n";
    pool=new ObjectsPool();
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
    background=NULL;
    area=NULL;
    player=NULL;
    name="";
}

int Scenario::loadMap(string mapname) {
    reinitMap();
    sfxeng->playMusic((config.datadir + "01theme.wav").c_str());
    name=mapname;
    ifstream mapfile;
    string tmpline;
    string image;
    string arg1,arg2,arg3;
    Uint16 x,y;
    background=NULL;
    area=NULL;
    string loadfile=config.datadir+name;
    bool header=false;
    
    mapfile.open(loadfile.c_str());
    if (mapfile) {
        cout << "Loading new map: " << loadfile << endl;
    } else {
        cout << "Map loading failed: " << loadfile << " not found!\n";
        return -2;
    }

    //check every line
    while (getline(mapfile,tmpline)) {
        //parse the header, always start a header with #HEADER and end it with #ENDHEADER!
        if (header) {
            name.erase();
            arg1.erase();
            arg2.erase();
            arg3.erase();
            std::istringstream tmpstream(tmpline);
            tmpstream >> name >> arg1 >> arg2 >> arg3;
            if (name[0]=='#') {
                if (name=="#ENDHEADER") header=false;
                continue;
            }
            continue;
        }
        name.erase();
        image.erase();
        x=0;
        y=0;
        arg1=arg2="0";
        arg3="";
        std::istringstream tmpstream(tmpline);
        tmpstream >> name >> image >> x >> y >> arg1 >> arg2 >> arg3;

        //Skip empty lines
        if (name.empty()) continue;
        //Skip comments
        if (name[0]=='#') {
            if (name=="#HEADER") header=true;
            continue;
        }

        if (name=="Background" && (!background)) {
            background=new Background(image);
            if (background) area=background->getPos();
        //Background has to be first!
        } else if (background) {
            pool->addObjectbyName(name,image,x,y,arg1,arg2,arg3);
        } else {
            cout << "No background found yet, skipping " << name << " ...\n";
        }
    }

    mapfile.close();
    mapfile.clear();

    //Has a background been found?
    if (background) {
        if (pool->playerspool.size()>0) player=*pool->playerspool.begin();
        else player=NULL;
        if (player==NULL) cout << "No player found!\n";
        return 0;
    } else {
       cout << "Map loading failed: No background found!\n";
       return -1;
    }
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
