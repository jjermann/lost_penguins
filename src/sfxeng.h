#ifndef _SOUNDSENGINE_H
#define _SOUNDSENGINE_H 1

/** \brief Sounds engine

    Plays or pauses the background music and all in play noises.
    Only the wav file format is supported.
*/
class SoundsEngine {
    public:
        SoundsEngine();
        ~SoundsEngine();
        void playWAV(Mix_Chunk* wav);
        void pauseMusic();
        void resumeMusic();
    private:
        Mix_Music* theme;
        bool disabled;
};

#endif
