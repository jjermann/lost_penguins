#include "lost_vikings.h"

using namespace std;


SoundCache::SoundCache() { }
SoundCache::~SoundCache() {
    map<string, Mix_Chunk*>::iterator sndit=sndcache.begin();
    while (sndit != sndcache.end()) {
        Mix_FreeChunk((*sndit).second);
        sndcache.erase(sndit++);
    }
}

Mix_Chunk* SoundCache::loadWAV(string soundfile) {
    map<string, Mix_Chunk*>::iterator sndit=sndcache.find(soundfile);
    
    //load new sound
    if (sndit==sndcache.end()) {
        #ifdef SDL_MIXER
            string loadfile=config.datadir+soundfile;
            Mix_Chunk* tmpwav=NULL;
            if ((tmpwav=Mix_LoadWAV(loadfile.c_str())) == NULL) {
                cout << "Couldn't load the wav file: " << soundfile << " (" << Mix_GetError() << ")" << endl;
                return NULL;
            }
            sndcache.insert(make_pair(soundfile,tmpwav));
            return tmpwav;
        #else
            return NULL;
        #endif
    //already there
    } else return (*sndit).second;
}
