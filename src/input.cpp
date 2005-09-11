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
#include "editor.h"

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
    if (game_mode&GAME_MENU) {
        pollMenuEvents();
    } else if (game_mode&GAME_PAUSED) {
        pollPausedEvents();
    } else if (game_mode&GAME_PLAY) {
        pollGameEvents();
    } else if (game_mode&GAME_EDIT) {
        pollEditEvents();
    } else {
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
                         if (game_mode&GAME_PLAY) {
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
                     removeGameMode(GAME_PAUSED);
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
                     addGameMode(GAME_PAUSED);
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
                } else if (key==config.keybind[KEY_DEBUG]) {
                     gfxeng->toggleShowDebug();
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

inline void InputHandler::pollEditEvents() {
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
            case SDL_MOUSEMOTION: {
                editor->run_action(EDIT_MOUSE_MOTION,event.motion.x,event.motion.y);
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                editor->run_action(editor->getActionMReleased(event.button.button),event.button.x,event.button.y);
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                editor->run_action(editor->getActionMPressed(event.button.button),event.button.x,event.button.y);
                break;
            }
            case SDL_KEYDOWN: {
                SDLKey key=event.key.keysym.sym;
                keypressed[key]=true;
                if (game_mode&GAME_TEXT_INPUT) {
                    Uint16 unikey=event.key.keysym.unicode;
                    if (unikey<127) editor->box->input(unikey);
                } else {
                    if (key==config.keybind[KEY_FULL]) {
                        gfxeng->toggleFullScreen();
                    } else if (key==config.keybind[KEY_MENU]) {
                        sfxeng->pauseMusic();
                        setMenu(new EditMenu());
                        gfxeng->update(UPDATE_ALL);
                    } else if (key==config.keybind[KEY_DEL]) {
                        editor->run_action(EDIT_REMOVE_OBJECTS);
                    } else if (key==config.keybind[KEY_QUIT]) {
                        quitGame(0);
                    } else if (key==config.keybind[KEY_NOANIM]) {
                        gfxeng->update(UPDATE_ALL);
                        if (game_mode&GAME_EDIT_NOANIM) {
                            removeGameMode(GAME_EDIT_NOANIM);
                        } else {
                            scenario->reloadMap();
                            addGameMode(GAME_EDIT_NOANIM);
                        }
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }    
    keystate = SDL_GetKeyState(NULL);
    if (keyState(KEY_LEFT)) {
        gfxeng->addShift(-SHIFT_SPEED,0);
    } else if (keyState(KEY_RIGHT)) {
        gfxeng->addShift(SHIFT_SPEED,0);
    } else if (keyState(KEY_UP)) {
        gfxeng->addShift(0,-SHIFT_SPEED);   
    } else if (keyState(KEY_DOWN)) {
        gfxeng->addShift(0,SHIFT_SPEED);
    }
}
