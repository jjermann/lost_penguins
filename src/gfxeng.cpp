#include "lost_vikings.h"

using namespace std;


GraphicsEngine::GraphicsEngine() {
    if ((screen=SDL_SetVideoMode(config.width,config.height,16,SDL_HWSURFACE|SDL_DOUBLEBUF|config.full)) != NULL) {
        cout << "Set VideoMode...\n";
    } else {
        cout << "Couldn't set VideoMode: " << SDL_GetError() << endl;
        quitGame(-1);
    }

    vbar=true;
    backpos.x=0;
    backpos.y=0;
    backpos.w=config.width;
    backpos.h=config.height-BAR_HEIGHT;
    //viking bar
    bar.x=0;
    bar.y=backpos.y+backpos.h;
    bar.w=config.width;
    bar.h=BAR_HEIGHT;
    lifeimage=new Animation(imgcache->loadImage("life.bmp"));
    
    Dframes=Dfps=currentfps=0;
    tcurrent=SDL_GetTicks();
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

inline void GraphicsEngine::drawVikingBar() {
    //#vikings
    Uint8 vnum=pool->vikingspool.size();
    //temporary dest pos, source pos, copy of dest pos
    SDL_Rect dpos,spos,vpos;
    dpos.h=BAR_HEIGHT;
    if (vnum>0) dpos.w=(Uint16)(config.width/vnum);
    else dpos.w=0;
    dpos.x=0;
    dpos.y=config.height-BAR_HEIGHT;
    //more temporary variables
    Item* item=NULL;
    Frame frame;

    //Fill the viking bar area
    SDL_FillRect(screen,&bar,SDL_MapRGB(screen->format,100,100,100));

    //draw each viking status
    viking_iterator vkit=pool->vikingspool.begin();
    while (vkit != pool->vikingspool.end()) {
        //temp. variables
        spos=(*vkit)->getIcon().pos;
        
        //enlight current viking
        if ((*vkit)==viking) {
            vpos=dpos;
            SDL_FillRect(screen,&vpos,SDL_MapRGB(screen->format,120,120,120));
        }
        
        //draw life points
        frame=lifeimage->getFrame();
        for (Uint8 j=0; j<(*vkit)->getHealth(); j++) {
            vpos=dpos;
            vpos.w=frame.pos.w;
            vpos.h=frame.pos.h;
            vpos.x+=j*(vpos.w+1);
            SDL_BlitSurface(frame.image,&frame.pos,screen,&vpos);
        }
        
        //draw viking
        vpos=dpos;
        vpos.y+=lifeimage->getFrame().pos.h+1;
        SDL_BlitSurface((*vkit)->getIcon().image,&spos,screen,&vpos);

        //Write text stuff
        //font->write(screen,itos(vik->getHealth()),dpos.x,dpos.y);
        font->write(screen,(*vkit)->getName(),dpos.x,config.height-font->getHeight());

        //draw the items of the current viking
        for (Uint8 j=0; j<MAX_ITEMS; j++) {
            vpos=dpos;
            vpos.x+=spos.w+1;
            vpos.y+=j*ICON_SIZE;
            vpos.w=ICON_SIZE;
            vpos.h=ICON_SIZE;
            item=(*vkit)->getItem(j);

            if ((j==(*vkit)->getItemNum()) && (*vkit)==viking) {
                SDL_FillRect(screen,&vpos,SDL_MapRGB(screen->format,200,200,200));
            }
            if (item) {
                frame=item->getFrame();
                SDL_BlitSurface(frame.image,&frame.pos,screen,&vpos);
            } //else TODO clear the area...
        }

        dpos.x+=dpos.w;
        ++vkit;
    }
}

void GraphicsEngine::renderScene(bool insist) {
    if (!paused || insist) {
        //We don't want to change pos!
        SDL_Rect tmprect,shift,srcpos;
        if (viking!=NULL) {
            shift=setShift(viking->getCenter());
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
    
        if (viking!=NULL) {
            tmprect=*(viking->getPos());
            srcpos=viking->getFrame().pos;
            shiftMapArea(tmprect,*(viking->getCurPos()));
            SDL_BlitSurface(viking->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
        }
    }
    
    //TODO don't draw the whole screen, just till bar, just upgrade certain regions of the bar
    if (vbar) drawVikingBar();
    if (fps) {
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

void GraphicsEngine::toggleVikingBar() {
    //on  -> off
    if (vbar) {
        vbar=false;
        backpos.h+=BAR_HEIGHT;
    //off -> on
    } else {
        vbar=true;
        backpos.h-=BAR_HEIGHT;
    }
}
