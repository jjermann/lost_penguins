#ifndef _SOUNDSENGINE_H_
    #define _SOUNDSENGINE_H_
#endif

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
