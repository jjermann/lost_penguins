#ifndef _SOUNDCACHE_H
#define _SOUNDCACHE_H 1

class SoundCache {
    public:
        SoundCache();
        ~SoundCache();
        Mix_Chunk* loadWAV(string soundfile);
    private:
        std::map<string,Mix_Chunk*> sndcache;    
};

#endif
