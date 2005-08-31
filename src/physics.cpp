#include "common.h"
#include "input.h"
#include "gfxeng.h"
#include "objectpools.h"
#include "players_common.h"
#include "scenario.h"
#include "physics.h"


PhysicHandler::PhysicHandler(): 
  tstart(SDL_GetTicks()),
  dt(0),
  reset_time(true),
  Dfps(0),
  Dframes(0),
  currentfps(0),
  minfps(1000) {
    tcurrent=tstart;
}

PhysicHandler::~PhysicHandler() {
}

void PhysicHandler::update() {
    if (game_mode&GAME_MENU) {
        reset_time=true;
    } else if (game_mode&GAME_PAUSED ) {
        reset_time=true;
        updatePaused();
    } else if (game_mode&GAME_PLAY) {
        if (reset_time) {
            Dfps=Dframes=currentfps=dt=0;
            minfps=1000;
        } else {
            dt=(SDL_GetTicks()-tcurrent);
            updateFPS();
        }
        reset_time=false;
        tcurrent=SDL_GetTicks();
        updateGame();
    } else if (game_mode&GAME_EDIT) {
        if (reset_time) {
            dt=0;
        } else {
            dt=(SDL_GetTicks()-tcurrent);
        }
        reset_time=false;
        tcurrent=SDL_GetTicks();
        updateEdit();
    } else {
    }
}

inline void PhysicHandler::updateGame() {
    object_iterator obit=scenario->pool->objectspool.begin();
    while (obit!=scenario->pool->objectspool.end()) {
        //remove marked objects
        if ((*obit)->isDeleted()) {
            obit=scenario->pool->removeObject(*obit);
        } else ++obit;
    }
    obit=scenario->pool->objectspool.begin();
    while (obit!=scenario->pool->objectspool.end()) {
        (*obit)->idle(dt);
        (*obit)->updateEvents(dt);
        ++obit;
    }
    //handle current (new) scenario->player

    if ((scenario->player!=NULL) && (!(scenario->player->getState(ESTATE_BUSY)))) {
        if (input->keyPressed(KEY_USE)) scenario->player->in_use();
        if (input->keyPressed(KEY_ACT)) scenario->player->in_act();
        if (input->keyPressed(KEY_LEFT))   scenario->player->in_left();
        if (input->keyPressed(KEY_RIGHT))  scenario->player->in_right();
        if (input->keyPressed(KEY_UP))     scenario->player->in_up();
        if (input->keyPressed(KEY_DOWN))   scenario->player->in_down();
        if (input->keyState(KEY_LEFT)) {
            scenario->player->unsetState(STATE_MRIGHT);
            scenario->player->setState(STATE_MLEFT);
        } else {
            scenario->player->unsetState(STATE_MLEFT);
        }
        if (input->keyState(KEY_RIGHT)) {
            scenario->player->unsetState(STATE_MLEFT);
            scenario->player->setState(STATE_MRIGHT);
        } else {
            scenario->player->unsetState(STATE_MRIGHT);
        }
        if ((!(scenario->player->getState(ESTATE_RUN)))||scenario->player->getState(ESTATE_ABORT)) {
            if (input->keyPressed(KEY_SP1))  scenario->player->in_sp1();
            if (input->keyPressed(KEY_SP2)) scenario->player->in_sp2();
        }

        if (input->keyState(KEY_LEFT))  scenario->player->in_left(dt);
        if (input->keyState(KEY_RIGHT)) scenario->player->in_right(dt);
        if (input->keyState(KEY_UP))    scenario->player->in_up(dt);
        if (input->keyState(KEY_DOWN))  scenario->player->in_down(dt);
    } else if (scenario->player!=NULL) {
        scenario->player->unsetState(STATE_MLEFT);
        scenario->player->unsetState(STATE_MRIGHT);
    }
    //run end scenario->player effects
    character_iterator cit=scenario->pool->characterspool.begin();
    while (cit!=scenario->pool->characterspool.end()) {
        (*cit)->fall(dt);
        (*cit)->updateAnimState(!((*cit)->getState(ESTATE_ANIM)));
        ++cit;
    }
    //update the animations of all objects
    obit=scenario->pool->objectspool.begin();
    while (obit!=scenario->pool->objectspool.end()) {
        if ((*obit)->getState(ESTATE_ANIM)) { 
            bool runs=(*obit)->updateAnim(dt);
            if (!runs) (*obit)->stopEvent();
        } else if ((*obit)->isRunning()) (*obit)->updateAnim(dt);
        ++obit;
    }
}

inline void PhysicHandler::updateEdit() {
    object_iterator obit=scenario->pool->objectspool.begin();
    while (obit!=scenario->pool->objectspool.end()) {
        //remove marked objects
        if ((*obit)->isDeleted()) {
            obit=scenario->pool->removeObject(*obit);
        } else ++obit;
    }
    obit=scenario->pool->objectspool.begin();
    while (obit!=scenario->pool->objectspool.end()) {
        (*obit)->idle(dt);
        (*obit)->updateEvents(dt);
        ++obit;
    }
    //run end scenario->player effects
    character_iterator cit=scenario->pool->characterspool.begin();
    while (cit!=scenario->pool->characterspool.end()) {
        (*cit)->fall(dt);
        (*cit)->updateAnimState(!((*cit)->getState(ESTATE_ANIM)));
        ++cit;
    }
    //update the animations of all objects
    obit=scenario->pool->objectspool.begin();
    while (obit!=scenario->pool->objectspool.end()) {
        if ((*obit)->getState(ESTATE_ANIM)) { 
            bool runs=(*obit)->updateAnim(dt);
            if (!runs) (*obit)->stopEvent();
        } else if ((*obit)->isRunning()) (*obit)->updateAnim(dt);
        ++obit;
    }
}

inline void PhysicHandler::updatePaused() {
    if (input->keyPressed(KEY_RIGHT)) {
        scenario->player->switchItem(true);
    }
    if (input->keyPressed(KEY_LEFT)) {
        scenario->player->switchItem(false);
    }
    if (input->keyPressed(KEY_DROP)) {
        scenario->player->dropItem();
        gfxeng->update(UPDATE_ALL);
    }
}

inline void PhysicHandler::updateFPS() {
    Dfps+=dt;
    ++Dframes;
    if (Dfps>=100) {
        currentfps=Uint16(Dframes*1000/Dfps);
        if (currentfps < minfps) minfps=currentfps;
        Dfps=Dframes=0;
    }

}
