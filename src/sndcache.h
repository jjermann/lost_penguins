#ifndef _SOUNDCACHE_H
#define _SOUNDCACHE_H 1

/** \brief Caches sounds.    
  
    Caches wav files (Mix_Chunks) according to their name
    (using a map container) and deletes them at the end.
*/
class SoundCache {
    public:
        SoundCache();
        ~SoundCache();
        Mix_Chunk* loadWAV(string soundfile);
    private:
        std::map<string,Mix_Chunk*> sndcache;    
};

#endif
