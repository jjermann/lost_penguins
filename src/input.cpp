#include "common.h"
#include "events.h"
#include "input.h"
#include "sndcache.h"
#include "gfxeng.h"
#include "sfxeng.h"
#include "scenario.h"
#include "objectpools.h"
#include "menu.h"
#include "physics.h"
#include "objects_common.h"


InputHandler::InputHandler() {
    au_pause=sndcache->loadWAV("pause.wav");
    SDL_PumpEvents();
    keystate = SDL_GetKeyState(NULL);
}

InputHandler::~InputHandler() {
}

bool InputHandler::keyPressed(ConfigKey key) {
    return keypressed[config.keybind[key]];
}
bool InputHandler::keyState(ConfigKey key) {
    return keystate[config.keybind[key]];
}   

void InputHandler::update() {
    if (menu) {
        pollMenuEvents();
    } else if (paused) {
        pollPausedEvents();
    } else {
        pollGameEvents();
    }
}

inline void InputHandler::pollMenuEvents() {
    for (Uint16 i=0; i<SDLK_LAST; i++) {
        keypressed[i]=false;
    }
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
           // special events
         case SDL_VIDEORESIZE: {
             gfxeng->resize(event.resize.w, event.resize.h);
             break;
         }
            // keyboard events
            case SDL_QUIT: {
                quitGame(0);
            }
            case SDL_KEYDOWN: {
                SDLKey key=event.key.keysym.sym;
                keypressed[key]=true;
                if (key==config.keybind[KEY_UP]) {
                    menu->increaseEntry(false);
                } else if (key==config.keybind[KEY_DOWN]) {
                    menu->increaseEntry();  
                } else if (key==config.keybind[KEY_ACT]) {
                    menu->act();
                } else if (key==config.keybind[KEY_MENU]) {
                     if (!closeMenu()) {
                         if (running) {
                             sfxeng->resumeMusic();
                         } else {
                             quitGame(0);
                         }
                     }
                } else if (key==config.keybind[KEY_FULL]) { 
                     gfxeng->toggleFullScreen();
                } else if (key==config.keybind[KEY_QUIT]) { 
                     quitGame(0);
                }
                gfxeng->update(UPDATE_MENU);
                break;
            }
            default: {
                break;
            }
        }
    }    
    keystate = SDL_GetKeyState(NULL);
}

inline void InputHandler::pollPausedEvents() {
    for (Uint16 i=0; i<SDLK_LAST; i++) {
        keypressed[i]=false;
    }
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
           // special events
         case SDL_VIDEORESIZE: {
             gfxeng->resize(event.resize.w, event.resize.h);
             break;
         }
            // keyboard events
            case SDL_QUIT: {
                quitGame(0);
            }
            case SDL_KEYDOWN: {
                SDLKey key=event.key.keysym.sym;
                keypressed[key]=true;
                if (key==config.keybind[KEY_SWITCH]) {
                     scenario->pool->switchPlayer();
                } else if (key==config.keybind[KEY_PAUSE]) {
                     paused=false;
                     sfxeng->resumeMusic();
                } else if (key==config.keybind[KEY_MENU]) {
                     sfxeng->pauseMusic();
                     setMenu(new GameMenu());
                } else if (key==config.keybind[KEY_FULL]) {
                     gfxeng->toggleFullScreen();
                } else if (key==config.keybind[KEY_QUIT]) {
                     quitGame(0);
                }
                gfxeng->update(UPDATE_BAR);
                break;
            }
            default: {
                break;
            }
        }
    }    
    keystate = SDL_GetKeyState(NULL);
}

inline void InputHandler::pollGameEvents() {
    for (Uint16 i=0; i<SDLK_LAST; i++) {
        keypressed[i]=false;
    }
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
           // special events
         case SDL_VIDEORESIZE: {
             gfxeng->resize(event.resize.w, event.resize.h);
             break;
         }
            // keyboard events
            case SDL_QUIT: {
                quitGame(0);
            }
            case SDL_KEYDOWN: {
                SDLKey key=event.key.keysym.sym;
                keypressed[key]=true;
                if (key==config.keybind[KEY_SWITCH]) {
                     scenario->pool->switchPlayer();
                } else if (key==config.keybind[KEY_PAUSE]) {  
                     paused=true;
                     sfxeng->playWAV(au_pause);
                     sfxeng->pauseMusic();
                } else if (key==config.keybind[KEY_MENU]) {
                     sfxeng->pauseMusic();
                     setMenu(new GameMenu());
                     gfxeng->update(UPDATE_ALL); 
                } else if (key==config.keybind[KEY_BAR]) {   
                     gfxeng->togglePlayerBar();
                } else if (key==config.keybind[KEY_FPS]) {
                     gfxeng->toggleFPS(); 
                } else if (key==config.keybind[KEY_FULL]) {
                     gfxeng->toggleFullScreen();
                } else if (key==config.keybind[KEY_QUIT]) {
                     quitGame(0);
                }
                break;
            }
            default: {
                break;
            }
        }
    }    
    keystate = SDL_GetKeyState(NULL);
}
