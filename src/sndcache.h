#ifndef DEF_SOUNDCACHE_H
#define DEF_SOUNDCACHE_H 1

/** \brief Caches sounds.    
  
    Caches wav files (Mix_Chunks) according to their name
    (using a map container) and deletes them at the end.
*/
class SoundCache {
    public:
        SoundCache();
        ~SoundCache();
        /// Loads a wav file if it isn't already contained in the pool
        /// according to it's file name.
        /// \return Pointer to the sound cache entry if the file was found and
        ///   could be loaded, NULL otherwise
        Mix_Chunk* loadWAV(string soundfile);
    private:
        std::map<string,Mix_Chunk*> sndcache;    
};

#endif
