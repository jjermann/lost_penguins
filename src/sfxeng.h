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
        /// Play the specified sound
        void playWAV(Mix_Chunk* wav);
        void playMusic(string);
        void stopMusic();
        void pauseMusic();
        void resumeMusic();
    private:
        /// Music: Theme of the current map
        Mix_Music* theme;
        /// True if sound support is disabled
        bool disabled;
};

#endif
