#include "common.h"
#include "events.h"
#include "anim.h"
#include "input.h"
#include "sndcache.h"
#include "gfxeng.h"
#include "sfxeng.h"
#include "objectpools.h"
//hack the player calls should be in anim.cpp
#include "objects_common.h"
#include "players_common.h"


InputHandler::InputHandler():
  state(NOTHING) {
    paused=false;
    au_pause=sndcache->loadWAV("pause.wav");
}

InputHandler::~InputHandler() {
}

void InputHandler::pollEvents() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT: {
                quitGame(0);
            }
            case SDL_KEYUP: {
                switch(event.key.keysym.sym) {
                    case SDLK_LEFT: {
                        if (state&INPUT_LEFT) player->in_left(-1);
                        state&=~INPUT_LEFT;
                        break;
                    }
                    case SDLK_RIGHT: {
                        if (state&INPUT_RIGHT) player->in_right(-1);
                        state&=~INPUT_RIGHT;
                        break;
                    }
                    case SDLK_UP: {
                        if (state&INPUT_UP) player->in_up(-1);
                        state&=~INPUT_UP;
                        break;
                    }
                    case SDLK_DOWN: {
                        if (state&INPUT_DOWN) player->in_down(-1);
                        state&=~INPUT_DOWN;
                        break;
                    }
                    case SDLK_SPACE: {
                        if (state&INPUT_SP1) player->in_sp1(-1);
                        state&=~INPUT_SP1;
                        break;
                    }
                    case SDLK_LSHIFT: {
                        if (state&INPUT_SP2) player->in_sp2(-1);
                        state&=~INPUT_SP2;
                        break;
                    }
                    case SDLK_TAB: {
                        state&=~INPUT_PAUSE;
                        break;
                    }
                    case SDLK_RETURN: {
                        if (state&INPUT_ACT) player->in_act(-1);
                        state&=~INPUT_ACT;
                        break;
                    }
                    case SDLK_INSERT: {
                        if (state&INPUT_USE) player->in_use(-1);
                        state&=~INPUT_USE;
                        break;
                    }
                    case SDLK_DELETE: {
                        state&=~INPUT_DEL;
                        break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            case SDL_KEYDOWN: {
                switch(event.key.keysym.sym) {
                    case SDLK_LCTRL: {
                        pool->switchPlayer();
                        state=NOTHING;
                        break;
                    }
                    case SDLK_LEFT: {
                        state|=INPUT_LEFT;
                        break;
                    }
                    case SDLK_RIGHT: {
                        state|=INPUT_RIGHT;
                        break;
                    }
                    case SDLK_UP: {
                        state|=INPUT_UP;
                        break;
                    }
                    case SDLK_DOWN: {
                        state|=INPUT_DOWN;
                        break;
                    }
                    case SDLK_SPACE: {
                        state|=INPUT_SP1;
                        break;
                    }
                    case SDLK_LSHIFT: {
                        state|=INPUT_SP2;
                        break;
                    }
                    case SDLK_RETURN: {
                        state|=INPUT_ACT;
                        break;
                    }
                    case SDLK_INSERT: {
                        state|=INPUT_USE;
                        break;
                    }
                    case SDLK_DELETE: {
                        state|=INPUT_DEL;
                        break;
                    }
                    //change game state!
                    case SDLK_TAB: {
                         if (state&INPUT_PAUSE) {
                             break;
                         } else if (paused) {
                             state|=INPUT_PAUSE;
                             paused=false;
                             anim->resetTime();
                             sfxeng->resumeMusic();
                         } else {
                             state|=INPUT_PAUSE;
                             paused=true;
                             sfxeng->playWAV(au_pause);
                             sfxeng->pauseMusic();
                         }
                         break;
                    }
                    case SDLK_F1: {
                         gfxeng->togglePlayerBar();
                         break;
                    }
                    case SDLK_F2: {
                         gfxeng->toggleFPS();
                         break;
                    }
                    case SDLK_q: {
                         quitGame(0);
                         break;
                    }
                    default: {
                        break;
                    }
                }
                break;
            }
            default: {
                break;
            }
        }
    }    
}


//SDL event filter
int filterEvents(const SDL_Event*) {
/*
int filterEvents(const SDL_Event *event) {
    switch(event->type) {
        case SDL_QUIT: {
            quitGame(0);
            break;
        }
        case SDL_KEYDOWN: {
            switch(event->key.keysym.sym) {
                case SDLK_p: {
                    input->resume();
                    break;
                }
                case SDLK_q: {
                    quitGame(0);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        default: {
            break;
        }
    }
*/
    return 1;
}
