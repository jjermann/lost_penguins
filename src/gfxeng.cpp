#include "common.h"
#include "font.h"
#include "animation.h"
#include "objectpools.h"
#include "players_common.h"
#include "imgcache.h"
#include "scenario.h"
#include "menu.h"
#include "editor.h"
//Only needed for fps
#include "scenario.h"
#include "physics.h"
#include "gfxeng.h"


GraphicsEngine::GraphicsEngine():
  screen(NULL),
  menubg(NULL),
  show_bar(true),
  show_fps(true),
  show_debug(false),
  fullscreen(config.full) {
    updatetype=UPDATE_ALL;
    shift.x=0;
    shift.y=0;
    resize(config.width, config.height);
    lifeimage=new Animation(imgcache->loadImage(1,"life.bmp"));
}

GraphicsEngine::~GraphicsEngine() {
    delete lifeimage;
    if (menubg) SDL_FreeSurface(menubg);
    SDL_FreeSurface(screen);
}

void GraphicsEngine::update(Uint8 upd) {
    if (updatetype!=UPDATE_ALL) updatetype=upd;
}

void GraphicsEngine::draw() {
    //Menu
    if (game_mode&GAME_MENU) {
        //Assure we have a (correct) menu background
        if (!menubg) {
            if (game_mode&(GAME_PLAY|GAME_EDIT)) {
                setGameMenuBG();
            } else {
                setMenuBG();
            }
        }
        if (updatetype==UPDATE_ALL) {
            if (game_mode&(GAME_PLAY|GAME_EDIT)) {
                setGameMenuBG();
            } else {
                setMenuBG();
            }
            drawMenu();
        } else if (updatetype==UPDATE_MENU) {
            drawMenu();
        }
    //Paused game
    } else if (game_mode&GAME_PAUSED) {
        if (updatetype==UPDATE_ALL) {
            drawScene();
            drawPlayerBar();
        } else if (updatetype==UPDATE_BAR) {
            drawPlayerBar();
        }
    //Not paused running game
    } else if (game_mode&GAME_PLAY) {
        drawScene();
        drawPlayerBar();
        drawFPS();
        updatetype=UPDATE_ALL;
    } else if (game_mode&GAME_EDIT) {
        if (show_fps) toggleFPS();
        if (show_bar) togglePlayerBar();
        if (!(game_mode&GAME_EDIT_NOANIM) || updatetype==UPDATE_ALL) {
            drawScene();
            drawBox();
            updatetype=UPDATE_ALL;
        }
    } else return;
    //This is the most time consuming operation
    if (updatetype!=UPDATE_NOTHING) SDL_Flip(screen);
    updatetype=UPDATE_NOTHING;
}

void GraphicsEngine::resize(Uint16 width, Uint16 height) {
    if (screen) SDL_FreeSurface(screen);
    if (fullscreen) vflags|=SDL_FULLSCREEN;
    else vflags&=~SDL_FULLSCREEN;

    if ((screen=SDL_SetVideoMode(width,height,0,vflags)) != NULL) {
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
    update(UPDATE_ALL);
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

void GraphicsEngine::drawScene() {
    //We don't want to change pos!
    SDL_Rect tmprect,srcpos,debugrect;
    if (game_mode&GAME_PLAY) {
        if (scenario->player!=NULL) {
            shift=setShift(scenario->player->getCenter());
        } else {
            shift.x=0;
            shift.y=0;
        }
    } else if (game_mode&GAME_EDIT) {
        SDL_FillRect(screen,NULL,0);
    }

    tmprect=*scenario->area;
    srcpos=scenario->background->getFrame().pos;
    SDL_BlitSurface(scenario->background->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
    
    object_iterator obit=scenario->pool->objectspool.begin();
    while (obit!=scenario->pool->objectspool.end()) {
        tmprect=((*obit)->getDrawPos());
        srcpos=(*obit)->getFrame().pos;
        if (show_debug) {
            debugrect=*(*obit)->getPos();
            drawRectangle(debugrect,1,SDL_MapRGB(screen->format,100,20,0));
        }
        SDL_BlitSurface((*obit)->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
        ++obit;
    }

    if (game_mode&GAME_PLAY && scenario->player!=NULL) {
        tmprect=(scenario->player->getDrawPos());
        srcpos=scenario->player->getFrame().pos;
        SDL_BlitSurface(scenario->player->getFrame().image,&srcpos,screen,shiftMapArea(tmprect,shift));
    }
}

//TODO don't draw the whole screen, just till bar, just upgrade certain regions of the bar
void GraphicsEngine::drawPlayerBar() {
    if (!show_bar) return;
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

void GraphicsEngine::drawFPS() {
    if (!show_fps) return;
    font->write(screen,"FPS: " + itos(scenario->physic->getFPS()) + " [" + itos(scenario->physic->getMinFPS()) + "]",screen->w-170,0);
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

void GraphicsEngine::setShowDebug() {
    show_debug=true;
}
void GraphicsEngine::unsetShowDebug() {
    show_debug=false;
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

void GraphicsEngine::drawMenu() {
    if (menubg) SDL_BlitSurface(menubg,NULL,screen,NULL);

    menu->font_title->writeCenter(screen,menu->title,0);

    //size of the menu in pixels if it was just one row
    Uint16 menu_pix_size   = (menu->getSize()-1)*DFONT+(menu->getSize()-1)*(menu->font->getHeight())+menu->font_high->getHeight();
    //number of rows to use
    Uint8  rows            = menu_pix_size/(screen->h-menu->font_title->getHeight()-DFONT)+1;
    //number of entries per row
    Uint8  row_size        = menu->getSize()/rows+1;
    //size of one row in pixels, differs from menu_pix_size/rows and depends on the currententry
    Uint16 row_pix_size    = (row_size-1)*DFONT+row_size*(menu->font->getHeight());
    Uint16 currow_pix_size = (row_size-1)*DFONT+(row_size-1)*(menu->font->getHeight())+menu->font_high->getHeight();
    //current height of the text
    Uint16 h;
    //current row
    Uint8  currow          = 0;
    //true if the currententry is in the current row
    bool   current_row     = false;

    for (Uint8 i=0; i< menu->getSize(); i++) {
        if (i>=row_size*(currow+1)) currow++;
        if (menu->currententry>=row_size*currow && menu->currententry<row_size*(currow+1)) current_row=true;
        else current_row=false;
        
        if (i<=menu->currententry || (!current_row)) {
            h=(screen->h+menu->font_title->getHeight()-(current_row ? currow_pix_size : row_pix_size))/2
              -DFONT
              +(i-row_size*currow)*DFONT
              +(i-row_size*currow)*(menu->font->getHeight());
        } else {
            h=(screen->h+menu->font_title->getHeight()-currow_pix_size)/2
              -DFONT
              +(i-row_size*currow)*DFONT
              +(i-1-row_size*currow)*(menu->font->getHeight())
              +(menu->font_high->getHeight());
        }
        if (i==menu->currententry) menu->font_high->writeCenter(screen,menu->entries[i],currow*(screen->w)/rows+(screen->w)/rows/2,h);
        else menu->font->writeCenter(screen,menu->entries[i],currow*(screen->w)/rows+(screen->w)/rows/2,h);
    }
}

void GraphicsEngine::drawBox() {
    if (editor->hasBox()) {
        SDL_Rect tmprect;
        tmprect=editor->box->area;
        SDL_BlitSurface(editor->box->surface,0,screen,&tmprect);
    }
}

//this could probably be done much easier ;)
inline void GraphicsEngine::setGameMenuBG() {
    if (menubg) SDL_FreeSurface(menubg);
    if (game_mode&GAME_PLAY) {
        drawScene();
        drawPlayerBar();
    } else if (game_mode&GAME_EDIT) drawScene();
    SDL_Flip(screen);
    
    SDL_Surface* tmp = SDL_CreateRGBSurface (
      SDL_HWSURFACE,
      screen->w,
      screen->h,
      32,
      rmask,gmask,bmask,0);
#ifdef ALPHA
    menubg = SDL_CreateRGBSurface (
      SDL_HWSURFACE,
      screen->w,
      screen->h,
      32,
      rmask,gmask,bmask,0);
    SDL_BlitSurface(screen,NULL,menubg,NULL);
    SDL_SetAlpha(menubg,SDL_SRCALPHA,60);
    SDL_FillRect(tmp,NULL,SDL_MapRGB(screen->format,0,0,0));
    SDL_BlitSurface(menubg,NULL,tmp,NULL);
    SDL_FreeSurface(menubg);
    menubg=SDL_DisplayFormatAlpha(tmp);
#else
    SDL_FillRect(tmp,NULL,SDL_MapRGB(screen->format,0,0,0));
    menubg=SDL_DisplayFormat(tmp);
#endif
    SDL_FreeSurface(tmp);
}

void GraphicsEngine::setMenuBG(SDL_Surface* menu_background) {
    if (menubg) SDL_FreeSurface(menubg);
    if (menu_background) {
        menubg=menu_background;
    } else {
        SDL_Surface* tmp = SDL_CreateRGBSurface (
          SDL_HWSURFACE,
          screen->w,
          screen->h,
          32,
          rmask,gmask,bmask,0);
        SDL_FillRect(tmp,NULL,SDL_MapRGB(screen->format,0,0,0));
        menubg=SDL_DisplayFormat(tmp);
    }
}

void GraphicsEngine::resetMenuBG() {
    if (menubg) {
        SDL_FreeSurface(menubg);
        menubg=NULL;
    }
}

const SDL_Rect& GraphicsEngine::addShift(Sint16 shiftx, Sint16 shifty) {
    shift.x+=shiftx;
    shift.y+=shifty;
    return shift;
}

const SDL_Rect& GraphicsEngine::setShift(Sint16 shiftx, Sint16 shifty) {
    shift.x=shiftx;
    shift.y=shifty; 
    return shift;   
}

void GraphicsEngine::drawRectangle(SDL_Rect rect, Uint8 border, Uint32 color) {
    SDL_Rect tmpline=*shiftMapArea(rect,shift);
    rect=tmpline;
    rect.x-=border;
    rect.y-=border;
    rect.w+=2*border;
    rect.h=border;
    /* horizontal top */
    SDL_FillRect(screen,&rect,color);
    rect=tmpline;
    rect.x-=border;
    rect.y+=tmpline.h;
    rect.w+=2*border;
    rect.h=border;
    /* horizontal bottom */
    SDL_FillRect(screen,&rect,color);
    rect=tmpline;
    rect.x-=border;
    rect.y-=border;
    rect.w=border;
    rect.h+=2*border;
    /* vertical left */
    SDL_FillRect(screen,&rect,color);
    rect=tmpline;
    rect.x+=tmpline.w;
    rect.y-=border;
    rect.w=border;
    rect.h+=2*border;
    /* vertical right */
    SDL_FillRect(screen,&rect,color);
}
