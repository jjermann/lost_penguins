#ifndef _SOUNDSENGINE_H
#define _SOUNDSENGINE_H 1

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
