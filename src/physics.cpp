#include "common.h"
#include "input.h"
#include "gfxeng.h"
#include "objectpools.h"
#include "players_common.h"
#include "scenario.h"
#include "physics.h"


PhysicHandler::PhysicHandler(): 
  tstart(SDL_GetTicks()),
  dt(0) {
    tcurrent=tstart;
}

PhysicHandler::~PhysicHandler() {
}

Uint16 PhysicHandler::resetTime() {
    dt=0;
    return (tcurrent=SDL_GetTicks());
}

void PhysicHandler::update() {
    dt=(SDL_GetTicks()-tcurrent);
    tcurrent=SDL_GetTicks();

    //Game is running normally
    if (!paused) {
        //released keys of player
        if (scenario->player != NULL) {
            if (input->getState(INPUTR_USE)) scenario->player->in_use(-1);
            input->unsetState(INPUTR_USE);
            if (input->getState(INPUTR_ACT)) scenario->player->in_act(-1);
            input->unsetState(INPUTR_ACT);
            if (input->getState(INPUTR_RIGHT)) scenario->player->in_right(-1);
            input->unsetState(INPUTR_RIGHT);
            if (input->getState(INPUTR_LEFT)) scenario->player->in_left(-1);
            input->unsetState(INPUTR_LEFT);
            if (input->getState(INPUTR_SP1)) scenario->player->in_sp1(-1);
            input->unsetState(INPUTR_SP1);
            if (input->getState(INPUTR_SP2)) scenario->player->in_sp2(-1);
            input->unsetState(INPUTR_SP2);
            if (input->getState(INPUTR_UP)) scenario->player->in_up(-1);
            input->unsetState(INPUTR_UP);
            if (input->getState(INPUTR_DOWN)) scenario->player->in_down(-1);
            input->unsetState(INPUTR_DOWN);
            input->unsetState(INPUTR_DEL);
        }
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
            if (input->getState(INPUT_USE)) scenario->player->in_use(dt);
            if (input->getState(INPUT_ACT)) scenario->player->in_act(dt);
            if (input->getState(INPUT_RIGHT)) scenario->player->in_right(dt);
            if (input->getState(INPUT_LEFT)) scenario->player->in_left(dt);
            if ((!(scenario->player->getState(ESTATE_RUN)))||scenario->player->getState(ESTATE_ABORT)) {
                if (input->getState(INPUT_SP1)) scenario->player->in_sp1(dt);
                if (input->getState(INPUT_SP2)) scenario->player->in_sp2(dt);
            }
            if (input->getState(INPUT_UP)) scenario->player->in_up(dt);
            if (input->getState(INPUT_DOWN)) scenario->player->in_down(dt);
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
    //Game is paused
    } else {
        if (input->getState(INPUT_RIGHT)) {
            input->unsetState(INPUT_RIGHT);
            scenario->player->switchItem(true);
        }
        if (input->getState(INPUT_LEFT)) {
            input->unsetState(INPUT_LEFT);
            scenario->player->switchItem(false);
        }
        if (input->getState(INPUT_DEL)) {
            input->unsetState(INPUT_DEL);
            scenario->player->dropItem();
            gfxeng->renderScene(true);
        }
    }
}
