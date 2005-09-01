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
    save_name="newmap.cfg";
    box=NULL;
    place_obj_name=NULL;
}

Editor::~Editor() {
    closeBox();
}

void Editor::run_action(Uint32 action, Uint16 x, Uint16 y) {
    SDL_Rect shift=gfxeng->getShift();
    Sint16 xs=x-shift.x;
    Sint16 ys=y-shift.y;

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
        if (scenario->pool->addObjectbyName(place_name,place_image,xs,ys,place_arg1,place_arg2,place_arg3)) {
            appendtoBuf(place_name+" "+place_image+" "+itos(xs)+" "+itos(ys)+" "+place_arg1+" "+place_arg2+" "+place_arg3);
        }
        if (place_obj_name) (*place_obj_name)=scenario->pool->getNextObjectName(place_name);
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
    closeBox();
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
  font_high(font2),
  centered(true) {  
    editor->run_action(EDIT_RESET_ACTIONS);
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
        if (centered) font->writeCenter(surface,entries[i],tmph);
        else font->write(surface,entries[i],WFONT,tmph);
        line.y=tmph+font->getHeight()+(int)((DFONT-line.h)/2);
        SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,180));
    }
}
 
EditBox::EditBox(Sint16 x, Sint16 y): Box(x,y) {
    title="EDIT MAP";
    entries.push_back("New");
    entries.push_back("Save As...");
    entries.push_back("Save");
    entries.push_back("Open");
    entries.push_back("Run");
    entries.push_back("Quit");
    entries.push_back("");
    entries.push_back("Place Object");
    entries.push_back("Remove Object");
    entries.push_back("Move Object");
    entries.push_back("Configure Object");
    update();
}

void EditBox::act(Sint8 curentry) {
    gfxeng->update(UPDATE_ALL);
    if (curentry==-1 || curentry >= (Sint8)entries.size()) {
        editor->closeBox();
    } else if (entries[curentry]=="New") {
        editor->setBox(new NewMapBox(area.x,area.y));
    } else if (entries[curentry]=="Save As...") {
        editor->setBox(new SaveAsBox(area.x,area.y));
    } else if (entries[curentry]=="Save") {
        if (editor->save_name=="newmap.cfg") {
            editor->setBox(new SaveAsBox(area.x,area.y));
        } else {
            editor->saveBuf(editor->save_name);
            editor->closeBox();
        }
    } else if (entries[curentry]=="Open") {
        editor->setBox(new OpenMapBox(area.x,area.y));
    } else if (entries[curentry]=="Run") {
        setGameMode(GAME_PLAY);
        scenario->reloadMap();
        editor->closeBox();
    } else if (entries[curentry]=="Place Object") {
        editor->setBox(new PlaceBox(area.x,area.y));
    } else if (entries[curentry]=="Remove Object") {
        editor->closeBox();
    } else if (entries[curentry]=="Move Object") {
        editor->closeBox();
    } else if (entries[curentry]=="Configure Object") {
        editor->closeBox();
    } else if (entries[curentry]=="Quit") {
        quitGame(0);
    } else {
        editor->closeBox();
    }
}

PlaceBox::PlaceBox(Sint16 x, Sint16 y): Box(x,y) {
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

void PlaceBox::act(Sint8 curentry) {
    gfxeng->update(UPDATE_ALL);
    if (curentry==-1 || curentry >= (Sint8)entries.size()) {
    } else {
        string next_name=scenario->pool->getNextObjectName(entries[curentry]);
        if        (entries[curentry]=="Wall") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"viking1.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Water") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"water.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Exit") {
            editor->setBox(new ObjectBox(entries[curentry],"exit.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Teleporter") {
            editor->place_obj_name=&editor->place_arg3;
            editor->setBox(new ObjectBox(entries[curentry],"viking1.bmp",area.x,area.y,"Teleport Destination (x coordinate): ","0","Teleport Destination (y coordinate): ","0","Name: ",next_name));
        } else if (entries[curentry]=="Wind") {
            editor->place_obj_name=&editor->place_arg2;
            editor->setBox(new ObjectBox(entries[curentry],"teleport_01.bmp",area.x,area.y,"Wind acceleration: ","0","Name: ",next_name));
        } else if (entries[curentry]=="Geyser") {
            editor->place_obj_name=&editor->place_arg2;
            editor->setBox(new ObjectBox(entries[curentry],"viking1.bmp",area.x,area.y,"Geyser force: ","0","Name: ",next_name));
        } else if (entries[curentry]=="Trigger") {
            editor->place_obj_name=&editor->place_arg2;
            editor->setBox(new ObjectBox(entries[curentry],"key.bmp",area.x,area.y,"Trigger target name: ","","Name: ",next_name,"Key name: ",""));
        } else if (entries[curentry]=="Door") {
            editor->place_obj_name=&editor->place_arg2;
            editor->setBox(new ObjectBox(entries[curentry],"viking1.bmp",area.x,area.y,"Key name: ","","Name: ",next_name));
        } else if (entries[curentry]=="Spike") {
            editor->place_obj_name=&editor->place_arg2;
            editor->setBox(new ObjectBox(entries[curentry],"viking1.bmp",area.x,area.y,"Direction (R=1,L=2,U=4,D=8): ","4","Name: ",next_name));
        } else if (entries[curentry]=="Heart") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"heart.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Key") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"key.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Bomb") {
            editor->place_obj_name=&editor->place_arg2;
            editor->setBox(new ObjectBox(entries[curentry],"bomb_fire.bmp",area.x,area.y,"Exploding time (in ms): ","0","Name: ",next_name));
        } else if (entries[curentry]=="Erik") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"viking.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Olaf") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"viking.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Baleog") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"viking.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Fang") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"viking.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Scorch") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"viking.bmp",area.x,area.y,"Name: ",next_name));
        } else if (entries[curentry]=="Plant") {
            editor->place_obj_name=&editor->place_arg2;
            editor->setBox(new ObjectBox(entries[curentry],"viking.bmp",area.x,area.y,"Recovery time (in ms): ","0","Name: ",next_name));
        } else if (entries[curentry]=="Zombie") {
            editor->place_obj_name=&editor->place_arg1;
            editor->setBox(new ObjectBox(entries[curentry],"viking.bmp",area.x,area.y,"Name: ",next_name));
        } else {
            editor->run_action(EDIT_RESET_ACTIONS);
            editor->closeBox();
        }
    }
}

TextInputBox::TextInputBox(Sint16 x,Sint16 y): Box(x,y),
  currententry(-1) {
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
    addGameMode(GAME_TEXT_INPUT);
    editor->action_mouse_released[SDL_BUTTON_RIGHT]=NOTHING;
}
TextInputBox::~TextInputBox() {
    SDL_EnableUNICODE(0);
    SDL_EnableKeyRepeat(0,SDL_DEFAULT_REPEAT_INTERVAL);
    removeGameMode(GAME_TEXT_INPUT);
    editor->action_mouse_released[SDL_BUTTON_RIGHT]=EDIT_BOX;
}
void TextInputBox::act(Sint8 button) {
    if (button>=0 && (Uint8)button<einput.size()) currententry=button;
}
void TextInputBox::input(Uint16 akey) {
    if (currententry==-1 || currententry>=(int)einput.size()) {
    } else if (akey==(Uint16)config.keybind[KEY_ACT]) {
        evaluateEntry();        
    // Backspace
    } else if (akey==8) {
        if ( einput[currententry].begin() != einput[currententry].end() ) {
            einput[currententry].erase(einput[currententry].end()-1);
            gfxeng->update(UPDATE_ALL);
            update();
        }
    } else if ((Uint16)akey<32 || (Uint16)akey>126) {
    } else {
        einput[currententry]+=(char)akey;
        gfxeng->update(UPDATE_ALL);
        update();
    }
}
void TextInputBox::evaluateEntry() {
    editor->closeBox();
}
void TextInputBox::update() {
    Uint8 maxs=max(entries.size(),max(einput.size(),etitles.size()));
    Uint16 maxw=0;
    string spaces="";
    if (entries.size()<maxs) entries.resize(maxs);
    if (etitles.size()<maxs) etitles.resize(maxs);
    for (Uint8 i=0;i<einput.size(); i++) {
        maxw=max(maxw,(Uint16)font->getTextWidth(etitles[i]));
    }
    for (Uint8 i=0; i<einput.size(); i++) {
        spaces="";
        while (font->getTextWidth(etitles[i]+spaces)<maxw) spaces+=" ";
        spaces+="  ";
        entries[i]=etitles[i]+spaces+einput[i];
    }
    for (Uint8 i=einput.size(); i<maxs; i++) {
        entries[i]=etitles[i];
    }
    Box::update();
}

SaveAsBox::SaveAsBox(Sint16 x, Sint16 y): TextInputBox(x,y) {
    title="Save As...";
    einput.push_back(editor->save_name);
    currententry=0;
    update();
}

void SaveAsBox::evaluateEntry() {
    editor->save_name=einput[0];
    editor->saveBuf(editor->save_name);
    editor->closeBox();
}

OpenMapBox::OpenMapBox(Sint16 x, Sint16 y): TextInputBox(x,y) {
    title="Open Map";
    einput.push_back(scenario->name);
    currententry=0;
    update();
}

void OpenMapBox::evaluateEntry() {
    scenario->loadMap(einput[0]);
    editor->closeBox();
}

NewMapBox::NewMapBox(Sint16 x, Sint16 y): TextInputBox(x,y) {
    title="New Map"; 
    centered=false; 
    etitles.push_back("Map file: "); einput.push_back("newmap.cfg");
    etitles.push_back("Background image: "); einput.push_back(scenario->bgimage);
    currententry=0;
    update();
}

void NewMapBox::evaluateEntry() {
    scenario->name=einput[0];
    scenario->newMap(einput[1]);
    scenario->reloadMap();
    editor->closeBox();
}

ObjectBox::ObjectBox(string name, string image, Sint16 x, Sint16 y, string targ1, string arg1, string targ2, string arg2, string targ3, string arg3): TextInputBox(x,y),
  objname(name) {
    title=name+" Properties";
    centered=false;
    currententry=0;
    etitles.push_back("Image name: "); einput.push_back(image);
    if (targ1!="" || arg1!="") {
        etitles.push_back(targ1);
        einput.push_back(arg1);
    }
    if (targ2!="" || arg2!="") {
        etitles.push_back(targ2);
        einput.push_back(arg2);
    }
    if (targ3!="" || arg3!="") {
        etitles.push_back(targ3);
        einput.push_back(arg3);
    }
    update();
}

void ObjectBox::evaluateEntry() {
    editor->place_name=objname;
    editor->place_image=einput[0];
    if (einput.size()>=2) editor->place_arg1=einput[1];
    if (einput.size()>=3) editor->place_arg2=einput[2];
    if (einput.size()>=4) editor->place_arg3=einput[3];
    editor->action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_PLACE_OBJECT;
    editor->closeBox();
}
