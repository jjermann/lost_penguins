#ifndef _SOUNDCACHE_H_
    #define _SOUNDCACHE_H_
#endif

class SoundCache {
    public:
        SoundCache();
        ~SoundCache();
        Mix_Chunk* loadWAV(string soundfile);
    private:
        std::map<string,Mix_Chunk*> sndcache;    
};
