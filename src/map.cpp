#include "common.h"
#include "objectpools.h"
#include "players_common.h"
#include "map.h"


//TODO: load map from a file
Map::Map(string mapname) {
    //Load map data
    if (loadMapData(mapname)==0) {
        cout << "Map loaded.\n";
    } else {
        quitGame(3);
    }
    
    if (pool->playerspool.size()>0) player=*pool->playerspool.begin();
    else player=NULL;
}

Map::~Map() {
}


//PRE:  map filename
//ACT:  parses the file, adds all objects, sets maparea
//      Map format (x,y = 0 if omitted):
//      <Object name> <Image name> <x position> <y position> <Arg1> <Arg2> <Arg3>
//      For normal options Arg1 is interpreted as object name (id)
//      For Teleporter Arg1,Arg3 are the destination coordinates
//      Example:
//      Background background.bmp
//      Teleporter teleporter.bmp 0 0 20 100 teleporter1
//POST: returns 0 if a background could be loaded, -1 otherwise
//TODO: better map format, eg. support headers
inline int Map::loadMapData(const string& mapname) {
    ifstream mapfile;
    string tmpline;
    string name, image;
    string arg1,arg2,arg3;
    Uint16 x,y;
    background=NULL;
    string loadfile=config.datadir+mapname;
    bool header=false;
    
    mapfile.open(loadfile.c_str());
    if (mapfile) {
        cout << "Loading map configuration file...\n";
    } else {
        cout << "Map loading failed: " << (config.datadir+mapname) << " not found!\n";
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
            if (background) maparea=background->getPos();
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
    if (background) return 0;
    else {
       cout << "Map loading failed: No background found!\n";
       return -1;
    }
}

//PRE:  movement (src,dest)
//POST: direction of movement, note: that may include an enlargment, etc...
Uint16 Map::getDirection(const SDL_Rect& src, const SDL_Rect& dest) const {
    Uint16 dir=NOTHING;
    if ((src.x+src.w) < (dest.x+dest.w)) dir|=DIR_RIGHT;
    if (src.x > dest.x)                  dir|=DIR_LEFT;
    if ((src.y+src.h) < (dest.y+dest.h)) dir|=DIR_DOWN;
    if (src.y > dest.y)                  dir|=DIR_UP;
    return dir;
}

//PRE:  rectangles src, dest
//ACT:  checks where src overlaps dest and where it touches dest
//POST: directions of overlapping
//      Note: DIR_ALL means that dest is contained in src
Hit Map::checkPlace(const SDL_Rect& src, const SDL_Rect& dest) const {
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


std::set<Character *> Map::getCharactersIn(Uint16 mask, const SDL_Rect& rect, bool touch, Uint16 distance, Uint16 dir) const {
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
