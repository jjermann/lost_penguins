#include "common.h"
#include "font.h"
#include "anim.h"
#include "objectpools.h"
#include "players_common.h"
#include "imgcache.h"
#include "gfxeng.h"


GraphicsEngine::GraphicsEngine():
  pbar(true),
  Dfps(0),
  Dframes(0),
  currentfps(0),
  tcurrent(SDL_GetTicks()),
  show_fps(true) {
    if ((screen=SDL_SetVideoMode(config.width,config.height,16,SDL_HWSURFACE|SDL_DOUBLEBUF|config.full)) != NULL) {
        cout << "Set VideoMode...\n";
    } else {
        cout << "Couldn't set VideoMode: " << SDL_GetError() << endl;
        quitGame(-1);
    }

    pbar=true;
    backpos.x=0;
    backpos.y=0;
    backpos.w=config.width;
    backpos.h=config.height-BAR_HEIGHT;
    //player bar
    bar.x=0;
    bar.y=backpos.y+backpos.h;
    bar.w=config.width;
    bar.h=BAR_HEIGHT;
    lifeimage=new Animation(imgcache->loadImage("life.bmp"));
}

GraphicsEngine::~GraphicsEngine() {
    delete lifeimage;
    SDL_FreeSurface(screen);
}


inline SDL_Rect GraphicsEngine::clipToBG(SDL_Rect dest) const {
    if (dest.x < backpos.x) {
        dest.w -= backpos.x-dest.x;
        dest.x = backpos.x;
    }
    if (dest.y < backpos.y) {
        dest.h = backpos.y - dest.y;
        dest.y = backpos.y;
    }
    return dest;
}

inline SDL_Rect* GraphicsEngine::shiftMapArea(SDL_Rect& area,const SDL_Rect& shift) {
    area.x+=shift.x;
    area.y+=shift.y;
    return &area;
}

inline SDL_Rect GraphicsEngine::setShift(SDL_Rect center) {
    SDL_Rect shift=center;
    shift.x=Uint16(backpos.w/2)-shift.x;
    shift.y=Uint16(backpos.h/2)-shift.y;
    if (maparea->w+shift.x<backpos.w) shift.x=backpos.w-maparea->w;
    if (maparea->h+shift.y<backpos.h) shift.y=backpos.h-maparea->h;
    if (shift.x>=0) shift.x=0;
    if (shift.y>=0) shift.y=0;
    backpos.x=-shift.x;
    backpos.y=-shift.y;
    return shift;    
}

inline void GraphicsEngine::drawPlayerBar() {
    //#players
    Uint8 pnum=pool->playerspool.size();
    //temporary dest pos, source pos, copy of dest pos
    SDL_Rect dpos,spos,ppos;
    dpos.h=BAR_HEIGHT;
    if (pnum>0) dpos.w=(Uint16)(config.width/pnum);
    else dpos.w=0;
    dpos.x=0;
    dpos.y=config.height-BAR_HEIGHT;
    //more temporary variables
    Item* item=NULL;
    Frame frame;

    //Fill the player bar area
    SDL_FillRect(screen,&bar,SDL_MapRGB(screen->format,100,100,100));

    //draw each player status
    player_iterator plit=pool->playerspool.begin();
    while (plit != pool->playerspool.end()) {
        //temp. variables
        spos=(*plit)->getIcon().pos;
        
        //enlight current player
        if ((*plit)==player) {
            ppos=dpos;
            SDL_FillRect(screen,&ppos,SDL_MapRGB(screen->format,120,120,120));
        }
        
        //draw life points
        frame=lifeimage->getFrame();
        for (Uint8 j=0; j<(*plit)->getHealth(); j++) {
            ppos=dpos;
            ppos.w=frame.pos.w;
            ppos.h=frame.pos.h;
            ppos.x+=j*(ppos.w+1);
            SDL_BlitSurface(frame.image,&frame.pos,screen,&ppos);
        }
        
        //draw player
        ppos=dpos;
        ppos.y+=lifeimage->getFrame().pos.h+1;
        SDL_BlitSurface((*plit)->getIcon().image,&spos,screen,&ppos);

        //Write text stuff
        //font->write(screen,itos(plr->getHealth()),dpos.x,dpos.y);
        font->write(screen,(*plit)->getName(),dpos.x,config.height-font->getHeight());

        //draw the items of the current player
        for (Uint8 j=0; j<MAX_ITEMS; j++) {
            ppos=dpos;
            ppos.x+=spos.w+1;
            ppos.y+=j*ICON_SIZE;
            ppos.w=ICON_SIZE;
            ppos.h=ICON_SIZE;
            item=(*plit)->getItem(j);

            if ((j==(*plit)->getItemNum()) && (*plit)==player) {
                SDL_FillRect(screen,&ppos,SDL_MapRGB(screen->format,200,200,200));
            }
            if (item) {
                frame=item->getFrame();
                SDL_BlitSurface(frame.image,&frame.pos,screen,&ppos);
            } //else TODO clear the area...
        }

        dpos.x+=dpos.w;
        ++plit;
    }
}

void GraphicsEngine::renderScene(bool insist) {
    if (!paused || insist) {
        //We don't want to change pos!
        SDL_Rect tmprect,shift,srcpos;
        if (player!=NULL) {
            shift=setShift(player->getCenter());
        } else {
            shift.x=0;
            shift.y=0;
        }
    
        tmprect=*maparea;
        srcpos=background->getFrame().pos;
        shiftMapArea(tmprect,*background->getCurPos());
        SDL_BlitSurface(background->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
        
        object_iterator obit=pool->objectspool.begin();
        while (obit!=pool->objectspool.end()) {
            tmprect=*((*obit)->getPos());
            srcpos=(*obit)->getFrame().pos;
            shiftMapArea(tmprect,*((*obit)->getCurPos()));
            SDL_BlitSurface((*obit)->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
            ++obit;
        }
    
        if (player!=NULL) {
            tmprect=*(player->getPos());
            srcpos=player->getFrame().pos;
            shiftMapArea(tmprect,*(player->getCurPos()));
            SDL_BlitSurface(player->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
        }
    }
    
    //TODO don't draw the whole screen, just till bar, just upgrade certain regions of the bar
    if (pbar) drawPlayerBar();
    if (show_fps) {
        Dfps+=(SDL_GetTicks()-tcurrent);
        tcurrent=SDL_GetTicks();
        ++Dframes;
        if (Dfps>=100) {
            currentfps=Uint16(Dframes*1000/Dfps);
            Dfps=0;
            Dframes=0;
        }
        font->write(screen,"FPS: " + itos(currentfps),config.width-150,0);
    }
    SDL_Flip(screen);
}

void GraphicsEngine::togglePlayerBar() {
    //on  -> off
    if (pbar) {
        pbar=false;
        backpos.h+=BAR_HEIGHT;
    //off -> on
    } else {
        pbar=true;
        backpos.h-=BAR_HEIGHT;
    }
}

void GraphicsEngine::toggleFPS() {
    //on  -> off
    if (show_fps) {
        show_fps=false;
    //off -> on
    } else {   
        show_fps=true;
    }
}
