#include "common.h"
#include "font.h"
#include "anim.h"
#include "objectpools.h"
#include "players_common.h"
#include "imgcache.h"
#include "map.h"
#include "gfxeng.h"


GraphicsEngine::GraphicsEngine():
  Dfps(0),
  Dframes(0),
  currentfps(0),
  tcurrent(SDL_GetTicks()),
  show_bar(true),
  show_fps(true),
  fullscreen(config.full) {
    resize(config.width, config.height);
    lifeimage=new Animation(imgcache->loadImage("life.bmp"));
}

GraphicsEngine::~GraphicsEngine() {
    delete lifeimage;
    SDL_FreeSurface(screen);
}

void GraphicsEngine::resize(Uint16 width, Uint16 height) {
    if (screen) SDL_FreeSurface(screen);
    
    if ((screen=SDL_SetVideoMode(width,height,config.bpp,SDL_HWSURFACE|SDL_RESIZABLE|SDL_DOUBLEBUF|(fullscreen ? SDL_FULLSCREEN : 0))) != NULL) {
    } else {
        cout << "Couldn't set VideoMode: " << SDL_GetError() << endl;
        quitGame(-1);
    }
    //player bar
    bar.w=screen->w;
    if (show_bar) bar.h=BAR_HEIGHT;
    else bar.h=0;
    bar.x=0;
    bar.y=screen->h-bar.h;
    vis_map.x=0;
    vis_map.y=0;
    vis_map.w=screen->w;
    vis_map.h=screen->h-bar.h;
}

inline SDL_Rect GraphicsEngine::clipToBG(SDL_Rect dest) const {
    if (dest.x < vis_map.x) {
        dest.w -= vis_map.x-dest.x;
        dest.x = vis_map.x;
    }
    if (dest.y < vis_map.y) {
        dest.h = vis_map.y - dest.y;
        dest.y = vis_map.y;
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
    shift.x=Uint16(vis_map.w/2)-shift.x;
    shift.y=Uint16(vis_map.h/2)-shift.y;
    if (scenario->area->w+shift.x<vis_map.w) shift.x=vis_map.w-scenario->area->w;
    if (scenario->area->h+shift.y<vis_map.h) shift.y=vis_map.h-scenario->area->h;
    if (shift.x>=0) shift.x=0;
    if (shift.y>=0) shift.y=0;
    vis_map.x=-shift.x;
    vis_map.y=-shift.y;
    return shift;    
}

inline void GraphicsEngine::drawPlayerBar() {
    //#players
    Uint8 pnum=scenario->pool->playerspool.size();
    //temporary dest pos, source pos, copy of dest pos
    SDL_Rect dpos,spos,ppos;
    dpos.h=BAR_HEIGHT;
    if (pnum>0) dpos.w=(Uint16)(screen->w/pnum);
    else dpos.w=0;
    dpos.x=0;
    dpos.y=screen->h-BAR_HEIGHT;
    //more temporary variables
    Item* item=NULL;
    Frame frame;

    //Fill the player bar area
    SDL_FillRect(screen,&bar,SDL_MapRGB(screen->format,100,100,100));

    //draw each player status
    player_iterator plit=scenario->pool->playerspool.begin();
    while (plit != scenario->pool->playerspool.end()) {
        //temp. variables
        spos=(*plit)->getIcon().pos;
        
        //enlight current player
        if ((*plit)==scenario->player) {
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
        font->write(screen,(*plit)->getName(),dpos.x,screen->h-font->getHeight());

        //draw the items of the current player
        for (Uint8 j=0; j<MAX_ITEMS; j++) {
            ppos=dpos;
            ppos.x+=spos.w+1;
            ppos.y+=j*ICON_SIZE;
            ppos.w=ICON_SIZE;
            ppos.h=ICON_SIZE;
            item=(*plit)->getItem(j);

            if ((j==(*plit)->getItemNum()) && (*plit)==scenario->player) {
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
    if (scenario->background && (!paused || insist)) {
        //We don't want to change pos!
        SDL_Rect tmprect,shift,srcpos;
        if (scenario->player!=NULL) {
            shift=setShift(scenario->player->getCenter());
        } else {
            shift.x=0;
            shift.y=0;
        }
    
        tmprect=*scenario->area;
        srcpos=scenario->background->getFrame().pos;
        shiftMapArea(tmprect,*scenario->background->getCurPos());
        SDL_BlitSurface(scenario->background->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
        
        object_iterator obit=scenario->pool->objectspool.begin();
        while (obit!=scenario->pool->objectspool.end()) {
            tmprect=*((*obit)->getPos());
            srcpos=(*obit)->getFrame().pos;
            shiftMapArea(tmprect,*((*obit)->getCurPos()));
            SDL_BlitSurface((*obit)->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
            ++obit;
        }
    
        if (scenario->player!=NULL) {
            tmprect=*(scenario->player->getPos());
            srcpos=scenario->player->getFrame().pos;
            shiftMapArea(tmprect,*(scenario->player->getCurPos()));
            SDL_BlitSurface(scenario->player->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
        }
    }
    
    //TODO don't draw the whole screen, just till bar, just upgrade certain regions of the bar
    if (show_bar) drawPlayerBar();
    if (show_fps) {
        Dfps+=(SDL_GetTicks()-tcurrent);
        tcurrent=SDL_GetTicks();
        ++Dframes;
        if (Dfps>=100) {
            currentfps=Uint16(Dframes*1000/Dfps);
            Dfps=0;
            Dframes=0;
        }
        font->write(screen,"FPS: " + itos(currentfps),screen->w-150,0);
    }
    SDL_Flip(screen);
}

void GraphicsEngine::togglePlayerBar() {
    //on  -> off
    if (show_bar) {
        show_bar=false;
        bar.h=0;
        vis_map.h=screen->h-bar.h;
    //off -> on
    } else {
        show_bar=true;
        bar.h=BAR_HEIGHT;
        vis_map.h=screen->h-bar.h;
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

void GraphicsEngine::toggleFullScreen() {
    //on  -> off
    if (fullscreen) {
        fullscreen=false;
        resize(screen->w,screen->h);
    //off -> on
    } else {   
        fullscreen=true;
        resize(screen->w,screen->h);
    }
}
