#include "common.h"
#include "scenario.h"
#include "objectpools.h"
#include "menu.h"
#include "font.h"
#include "gfxeng.h"
#include "editor.h"


Editor::Editor() {
    run_action(EDIT_RESET_ACTIONS);
    string place_name="";
    save_name="newmap.txt";
    box=NULL;
}

Editor::~Editor() {
    closeBox();
    saveBuf(save_name);
}

void Editor::run_action(Uint32 action, Uint16 x, Uint16 y) {
    gfxeng->update(UPDATE_ALL);
    if (action&EDIT_RESET_ACTIONS) {
        for (Uint8 i=0; i<6; i++) {
            action_mouse_pressed[i]=NOTHING;
            action_mouse_released[i]=NOTHING;
        }
        action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_ACT_BOX;
        action_mouse_pressed[SDL_BUTTON_RIGHT]=EDIT_BOX;
        action_mouse_released[SDL_BUTTON_RIGHT]=EDIT_ACT_BOX;
    } else if (action&EDIT_BOX) {
        setBox(new EditBox(x,y));
    } else if (action&EDIT_ACT_BOX) {
        if (box) box->act(box->getCurrentEntry(x,y));
    } else if (action&EDIT_PLACE_OBJECT) {
        scenario->reloadMap();
        string next_name=scenario->pool->getNextObjectName(place_name);
        if (scenario->pool->addObjectbyName(place_name,place_image,x,y,next_name)) {
            appendtoBuf(place_name+" "+place_image+" "+itos(x)+" "+itos(y)+" "+next_name);
        }
//        action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_ACT_BOX;
    } else { }
}

void Editor::appendtoBuf(string line) {
    scenario->mapbuf.push_back(line);
}

string Editor::removefromBuf(string match) {
    for (Uint16 linenum=0; linenum<scenario->mapbuf.size(); linenum++) {
        if (scenario->mapbuf[linenum].find(match)!=string::npos) {
            vector<string>::iterator it=scenario->mapbuf.begin()+linenum;
            string returnstring=scenario->mapbuf[linenum];
            scenario->mapbuf.erase(it);
            return returnstring;
        }
    }
    return "";
}

int Editor::saveBuf(string filename) {
    ofstream outfile;
    outfile.open(filename.c_str());
    if (outfile.is_open()) {
        for (Uint16 linenum=0; linenum<scenario->mapbuf.size(); linenum++) {
            outfile << scenario->mapbuf[linenum] << endl;
        }
        outfile.close();
        cout << "Saved map to " << filename << endl;
        return 0;
    } else {
        cout << "Failed to save map to " << filename << ": Could not open file!" << endl;
        return -1;
    }
}

Box* Editor::setBox(Box* newbox) {
    gfxeng->update(UPDATE_ALL);
    return box=newbox;   
}
 
void Editor::closeBox() {
    gfxeng->update(UPDATE_ALL);
    if (box) delete box;
    box=NULL;
}
 
Box::Box(Sint16 x, Sint16 y):
  title("MENU"),
  surface(NULL),
  //don't forget the "::"
  font(::font),
  font_title(font),
  font_high(font2) {  
    area.x=x;
    area.y=y;
}
Box::~Box() {
}
 
void Box::act(Sint8) {
    gfxeng->update(UPDATE_ALL);
    editor->closeBox();
}
 
const SDL_Rect& Box::getArea() {
    Uint16 tmp=0;
    Uint16 maxwidth=font_title->getTextWidth(title);
    for (Uint8 i=0; i<entries.size(); i++) {
        tmp=max(font->getTextWidth(entries[i]),font_high->getTextWidth(entries[i]));
        maxwidth=max(maxwidth,tmp);
    }
    area.w=maxwidth+2*WFONT;
    area.h=entries.size()*(DFONT+font->getHeight())+2*WFONT+font_title->getHeight();
    return area;
}
 
Sint8 Box::getCurrentEntry(Sint16 x, Sint16 y) {
    getArea();
    if (
      (x>=(area.x+area.w)) ||
      (x<=area.x) ||
      (y>=(area.y+area.h)) ||
      (y<=(area.y+WFONT+font_title->getHeight()+(int)(DFONT/2)))) {
        return -1;
    }
    Sint16 tmp=y-area.y-WFONT-font_title->getHeight()-(int)(DFONT/2);
    Uint16 entrysize=DFONT+font->getHeight();
    Uint8 currententry=0;
    while (tmp>entrysize) {
        tmp-=entrysize;
        ++currententry;
    }
    return currententry;
}
 
void Box::update() {
    gfxeng->update(UPDATE_ALL);
    getArea();

    if (surface!=NULL) SDL_FreeSurface(surface);
    SDL_Surface* tmp=SDL_CreateRGBSurface(vflags, area.w, area.h, 32, rmask, gmask, bmask, amask);
    surface=SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);

    SDL_FillRect(surface,0,SDL_MapRGBA(surface->format,200,200,200,180));
    /* create a border */
    Sint16 tmph=0;
    SDL_Rect line;
    line.x=0;
    line.y=0;
    line.w=BORDERSIZE;
    line.h=area.h;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,255));
    line.x=area.w-BORDERSIZE;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,255));
    line.x=0;
    line.y=0;
    line.w=area.w;
    line.h=BORDERSIZE;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,255));
    line.y=area.h-BORDERSIZE;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,255));

    /* write title */
    font_title->writeCenter(surface,title,WFONT);
    line.y=font_title->getHeight()+(int)((DFONT-line.h)/2);
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,255));
    line.h=LINESIZE;

    /* write entries */
    for (Uint8 i=0; i<entries.size(); i++) {
        tmph=DFONT*(i+1)+font->getHeight()*i+WFONT+font_title->getHeight();
        font->writeCenter(surface,entries[i],tmph);
        line.y=tmph+font->getHeight()+(int)((DFONT-line.h)/2);
        SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,180));
    }
}
 
EditBox::EditBox(Sint16 x, Sint16 y): Box(x,y) {
    title="PLACE";
    entries.push_back("Wall");
    entries.push_back("Exit");
    entries.push_back("Water");
    entries.push_back("Teleporter");
    entries.push_back("Wind");
    entries.push_back("Geyser");
    entries.push_back("Trigger");
    entries.push_back("Door");   
    entries.push_back("Spike");  

    entries.push_back("Heart");
    entries.push_back("Key");  
    entries.push_back("Bomb"); 

    entries.push_back("Erik");
    entries.push_back("Olaf");
    entries.push_back("Baleog");
    entries.push_back("Fang");  
    entries.push_back("Scorch");

    entries.push_back("Plant");
    entries.push_back("Zombie");

    update();
}

void EditBox::act(Sint8 curentry) {
    gfxeng->update(UPDATE_ALL);
    if (curentry==-1 || curentry >= (Sint8)entries.size()) {
        editor->closeBox();
    } else {
        editor->place_name=entries[curentry];
        if        (editor->place_name=="Wall") {
            editor->place_image="viking1.bmp";
        } else if (editor->place_name=="Water") {
            editor->place_image="water.bmp";
        } else if (editor->place_name=="Exit") {
            editor->place_image="exit.bmp";
        } else if (editor->place_name=="Teleporter") {
            editor->place_image="viking1.bmp";
        } else if (editor->place_name=="Wind") {
            editor->place_image="teleport_01.bmp";
        } else if (editor->place_name=="Geyser") {
            editor->place_image="viking1.bmp";
        } else if (editor->place_name=="Trigger") {
            editor->place_image="key.bmp";
        } else if (editor->place_name=="Door") {
            editor->place_image="viking1.bmp";
        } else if (editor->place_name=="Spike") {
            editor->place_image="viking1.bmp";
        } else if (editor->place_name=="Heart") {
            editor->place_image="heart.bmp";
        } else if (editor->place_name=="Key") {
            editor->place_image="key.bmp";
        } else if (editor->place_name=="Bomb") {
            editor->place_image="bomb_fire.bmp";
        } else if (editor->place_name=="Erik") {
            editor->place_image="viking.bmp";
        } else if (editor->place_name=="Olaf") {
            editor->place_image="viking.bmp";
        } else if (editor->place_name=="Baleog") {
            editor->place_image="viking.bmp";
        } else if (editor->place_name=="Fang") {
            editor->place_image="viking.bmp";
        } else if (editor->place_name=="Scorch") {
            editor->place_image="viking.bmp";
        } else if (editor->place_name=="Plant") {
            editor->place_image="viking1.bmp";
        } else if (editor->place_name=="Zombie") {
            editor->place_image="viking.bmp";
        } else {
            editor->place_image="";
        }
        editor->action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_PLACE_OBJECT;
        editor->closeBox();
    }
}

TextInputBox::TextInputBox(Sint16 x,Sint16 y): Box(x,y),
  currententry(-1) {
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
    addGameMode(GAME_TEXT_INPUT);
}
TextInputBox::~TextInputBox() {
    SDL_EnableUNICODE(0);
    SDL_EnableKeyRepeat(0,SDL_DEFAULT_REPEAT_INTERVAL);
    removeGameMode(GAME_TEXT_INPUT);
    editor->run_action(EDIT_RESET_ACTIONS);
}

void TextInputBox::act(Sint8 button) {
    if (button>=0 && (Uint8)button<entries.size()) currententry=button;
}
void TextInputBox::input(Uint16 akey) {
    if (currententry==-1) {
    } else if (akey==(Uint16)config.keybind[KEY_ACT]) {
        editor->closeBox();
    // Backspace
    } else if (akey==8) {
        if ( entries[currententry].begin() != entries[currententry].end() ) {
            entries[currententry].erase(entries[currententry].end()-1);
            gfxeng->update(UPDATE_ALL);
            update();
        }
    } else if ((Uint16)akey<32 || (Uint16)akey>126) {
    } else {
        entries[currententry]+=(char)akey;
        gfxeng->update(UPDATE_ALL);
        update();
    }
}
