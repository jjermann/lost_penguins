#include "common.h"
#include "sfxeng.h"


SoundsEngine::SoundsEngine() {
#ifdef SDL_MIXER
    /* Open the audio device */
    if (Mix_OpenAudio(config.audio_rate, config.audio_format, config.audio_channels, 512) < 0) {
        cout << "Couldn't open audio: " <<  SDL_GetError() << endl;
        cout << "Audio disabled!\n";
        disabled=true;
    } else {
        Mix_QuerySpec(&config.audio_rate, &config.audio_format, &config.audio_channels);
        cout << "Opened audio at " << config.audio_rate << " Hz " << (config.audio_format&0xFF) << " bit " << ((config.audio_channels > 1) ? "stereo" : "mono") << endl;
        disabled=false;
    }
    theme=NULL;
#endif
}

SoundsEngine::~SoundsEngine() {
#ifdef SDL_MIXER
    if(!disabled) {
        Mix_CloseAudio();
        Mix_FreeMusic(theme);
    }
#endif
}
void SoundsEngine::playMusic(string name) {
#ifdef SDL_MIXER
    if (!disabled) {
        if (theme) Mix_FreeMusic(theme);
        theme = Mix_LoadMUS(name.c_str());
        Mix_FadeInMusic(theme, -1, 5000);
        Mix_VolumeMusic(MIX_MAX_VOLUME/4);
    }
#endif
}
void SoundsEngine::stopMusic() {
#ifdef SDL_MIXER
    if (!disabled) {
        if (theme) Mix_FreeMusic(theme);
        theme=NULL;
    }
#endif
}
void SoundsEngine::pauseMusic() {
#ifdef SDL_MIXER
    if (!disabled) Mix_PauseMusic();
#endif
}

void SoundsEngine::resumeMusic() {
#ifdef SDL_MIXER
    if (!disabled) Mix_ResumeMusic();
#endif
}

void SoundsEngine::playWAV(Mix_Chunk* wav) {
#ifdef SDL_MIXER
    if (wav) Mix_PlayChannel(-1, wav, 0);
#endif
}
