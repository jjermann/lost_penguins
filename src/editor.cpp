#include "common.h"
#include "scenario.h"
#include "objectpools.h"
#include "menu.h"
#include "font.h"
#include "gfxeng.h"
#include "input.h"
#include "objects_common.h"
#include "editor.h"


Editor::Editor() {
    string place_name="";
    save_name="newmap.cfg";
    box=NULL;
    mask_surface=NULL;
    move_object=NULL;
    reinit();
    run_action(EDIT_RESET_ACTIONS);
}

Editor::~Editor() {
    if (move_object) delete move_object;
    closeBox();
}

void Editor::reinit() {
    if (mask_surface) SDL_FreeSurface(mask_surface);
    mask_surface=gfxeng->createRGBAScreenSurface();
    //128 is a special case for an alpha value => faster
    SDL_FillRect(mask_surface,0,SDL_MapRGBA(mask_surface->format,100,0,0,128));
    if (box) box->update();
}

void Editor::updateSelection(Sint16 x, Sint16 y) {
    select_rect.w=abs(x-select_start_x);
    select_rect.h=abs(y-select_start_y);
    if (x>select_start_x) select_rect.x=select_start_x;
    else select_rect.x=x;
    if (y>select_start_y) select_rect.y=select_start_y;
    else select_rect.y=y;
}

bool Editor::updateMove(Sint16 x, Sint16 y) {
    if (move_object) return move_object->setPos(x,y);
    else return false;
}

void Editor::run_action(Uint32 action, Uint16 x, Uint16 y) {
    SDL_Rect shift=gfxeng->getShift();
    Sint16 xs=x-shift.x;
    Sint16 ys=y-shift.y;

    gfxeng->update(UPDATE_ALL);
    if (action&EDIT_MOUSE_MOTION) {
        if (select_start) {
            updateSelection(xs,ys);
            gfxeng->update(UPDATE_ALL);
        }
        if (move_start) {
            updateMove(xs,ys);
            gfxeng->update(UPDATE_ALL);
        }
    } else if (action&EDIT_RESET_ACTIONS) {
        for (Uint8 i=0; i<6; i++) {
            action_mouse_pressed[i]=NOTHING;
            action_mouse_released[i]=NOTHING;
        }
        action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_SEL_ACT_BOX;
        action_mouse_released[SDL_BUTTON_LEFT]=EDIT_SELECT_END;
        action_mouse_pressed[SDL_BUTTON_RIGHT]=EDIT_BOX;
        action_mouse_released[SDL_BUTTON_RIGHT]=EDIT_ACT_BOX;
        select_start=false;
        move_start=false;
        // HACK
        if (move_object) delete move_object;
        move_object=NULL;
    } else if (action&EDIT_BOX) {
        setBox(new EditBox(x,y));
    } else if (action&EDIT_SEL_ACT_BOX) {
        if (box) {
            Sint8 tmp=box->getCurrentEntry(x,y);
            if (tmp==-1) run_action(EDIT_SELECT_START,x,y);
            else box->act(tmp);
        } else {
            run_action(EDIT_SELECT_START,x,y);
        }
    } else if (action&EDIT_ACT_BOX) {
        Sint8 tmp=box->getCurrentEntry(x,y);
        if (box) box->act(tmp);
    } else if (action&EDIT_REMOVE_OBJECTS) {
        std::set<string>::iterator sit=selection.begin();
        while (sit!=selection.end()) {
            if (removefromBuf(*sit).empty()) {
                cout << "Unable to remove object " << (*sit) << endl;
            }
            ++sit;
        }
        scenario->reloadMap();
    } else if (action&EDIT_PLACE_OBJECT) {
        updateMove(xs,ys);
        if (scenario->pool->addObject(move_object)) {
            appendtoBuf(place_name+" "+itos(xs)+" "+itos(ys)+" "+putParameters(place_parameters));
        }
        place_parameters["name"]=scenario->pool->getNextObjectName(place_name);
        editor->move_object=scenario->pool->addObjectbyName(place_name,-1000,-1000,place_parameters,true);
        if (editor->move_object) editor->move_start=true;
        else editor->move_start=false;
    } else if (action&EDIT_SELECT_START) {
        select_start=true;
        select_start_x=xs;
        select_start_y=ys;
        select_rect.x=select_start_x;
        select_rect.y=select_start_y;
        select_rect.w=select_rect.h=0;
    } else if (action&EDIT_SELECT_END) {
        updateSelection(xs,ys);
        select_start=false;
        std::set<Object*> objects_sel;
        Object* obj=NULL;
        if (select_rect.w==0 && select_rect.h==0) {
            if ((obj=scenario->getObjectAt(xs,ys))!=NULL) objects_sel.insert(obj);
        } else {
            objects_sel=scenario->getObjectsIn(ALL,select_rect);
        }
        object_iterator obit=objects_sel.begin();

        if (input->keyState(KEY_ADD_SEL)) {
            while (obit!=objects_sel.end()) {
                selection.insert((*obit)->getName());
                ++obit;
                                      }
        } else if (input->keyState(KEY_RM_SEL)) {
            while (obit!=objects_sel.end()) {
                selection.erase((*obit)->getName());
                ++obit;
            }
        } else {
            selection.clear();
            while (obit!=objects_sel.end()) {
                selection.insert((*obit)->getName());
                ++obit;
            }
        }
    } else if (action&EDIT_MOVE_START) {
        move_start_x=xs;
        move_start_y=ys;
        // HACK
        if (move_object) delete move_object;
        if ((move_object=scenario->pool->moveObject(scenario->getObjectAt(xs,ys)))) move_start=true;
        else move_start=false;
    } else if (action&EDIT_MOVE_END) {
        if (move_object==NULL) {
            move_start=false;
            return;
        }
        updateMove(xs,ys);
        if (scenario->pool->addObject(move_object)) {
            istringstream tmpstream(getBufLine(move_object->getName()));
            string name, x, y;
            tmpstream >> name >> x >> y;
            string nss((istreambuf_iterator<char>(tmpstream)), istreambuf_iterator<char>());
            string newline=name+" "+itos(move_object->getPos()->x)+" "+itos(move_object->getPos()->y)+" "+nss;
            changeBuf(newline,move_object->getName());
        } else {
            move_object->setPos(move_start_x,move_start_y);
            if (!scenario->pool->addObject(move_object)) {
                delete move_object;
                move_object=NULL;
                move_start=false;
                scenario->reloadMap();
            }
        }
        move_object=NULL;
        move_start=false;
    } else { }
}

void Editor::appendtoBuf(string line) {
    scenario->mapbuf.push_back(line);
}
Sint16 Editor::getBufLineNr(string match) {
    for (Uint16 linenum=0; linenum<scenario->mapbuf.size(); linenum++) {
        if (scenario->mapbuf[linenum].find(match)!=string::npos) return linenum;
    }
    return -1;
}
string Editor::getBufLine(string match) {
    for (Uint16 linenum=0; linenum<scenario->mapbuf.size(); linenum++) {
        if (scenario->mapbuf[linenum].find(match)!=string::npos) {
            return scenario->mapbuf[linenum];
        }
    }
    return "";
}
bool Editor::changeBuf(string newline, string match) {
    Sint16 linenum=getBufLineNr(match);
    if (linenum<0) return false;
    if (linenum>=(int)scenario->mapbuf.size()) return false;
    scenario->mapbuf[linenum]=newline;
    return true;
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

    area.x+=min(0,(int)(scenario->area->w-area.x-area.w));
    area.y+=min(0,(int)(scenario->area->h-area.y-area.h));
    if (area.x<0) area.x=0;
    if (area.y<0) area.y=0;
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
    Sint8 currententry=0;
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
    surface=gfxeng->createRGBASurface(area.w, area.h);

    SDL_FillRect(surface,0,SDL_MapRGBA(surface->format,200,200,200,128));
    /* create a border */
    Sint16 tmph=0;
    SDL_Rect line;
    line.x=0;
    line.y=0;
    line.w=BORDERSIZE;
    line.h=area.h;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,SDL_ALPHA_OPAQUE));
    line.x=area.w-BORDERSIZE;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,SDL_ALPHA_OPAQUE));
    line.x=0;
    line.y=0;
    line.w=area.w;
    line.h=BORDERSIZE;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,SDL_ALPHA_OPAQUE));
    line.y=area.h-BORDERSIZE;
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,SDL_ALPHA_OPAQUE));

    /* write title */
    font_title->writeCenter(surface,title,WFONT);
    line.y=font_title->getHeight()+(int)((DFONT-line.h)/2);
    SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,SDL_ALPHA_OPAQUE));
    line.h=LINESIZE;

    /* write entries */
    for (Uint8 i=0; i<entries.size(); i++) {
        tmph=DFONT*(i+1)+font->getHeight()*i+WFONT+font_title->getHeight();
        if (centered) font->writeCenter(surface,entries[i],tmph);
        else font->write(surface,entries[i],WFONT,tmph);
        line.y=tmph+font->getHeight()+(int)((DFONT-line.h)/2);
        SDL_FillRect(surface,&line,SDL_MapRGBA(surface->format,100,100,100,128));
    }
}
 
EditBox::EditBox(Sint16 x, Sint16 y): Box(x,y) {
    title="EDIT MAP";
    entries.push_back("Reload");
    entries.push_back("New");
    entries.push_back("Save As...");
    entries.push_back("Save");
    entries.push_back("Open");
    entries.push_back("Run");
    entries.push_back("Quit");
    entries.push_back("");
    entries.push_back("Place Objects");
    entries.push_back("Select Objects");
    entries.push_back("Move Object");
    update();
}

void EditBox::act(Sint8 curentry) {
    gfxeng->update(UPDATE_ALL);
    if (curentry==-1 || curentry >= (Sint8)entries.size()) {
        editor->closeBox();
    } else if (entries[curentry]=="Reload") {
        scenario->reloadMap();
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
    } else if (entries[curentry]=="Place Objects") {
        editor->setBox(new PlaceBox(area.x,area.y));
    } else if (entries[curentry]=="Select Objects") {
        editor->action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_SELECT_START;
        editor->action_mouse_released[SDL_BUTTON_LEFT]=EDIT_SELECT_END;
        editor->closeBox();
    } else if (entries[curentry]=="Move Object") {
        editor->action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_MOVE_START;
        editor->action_mouse_released[SDL_BUTTON_LEFT]=EDIT_MOVE_END;
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
        ParameterMap parameters=scenario->pool->getDefaultObjParambyName(entries[curentry]);
        parameters["name"]=scenario->pool->getNextObjectName(entries[curentry]);
        editor->setBox(new ObjectBox(entries[curentry],area.x,area.y,parameters));
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
    editor->saveBuf(config.datadir+editor->save_name);
    editor->closeBox();
}

OpenMapBox::OpenMapBox(Sint16 x, Sint16 y): TextInputBox(x,y) {
    title="Open Map";
    currententry=0;
    einput.push_back(scenario->mapname);
    update();
}

void OpenMapBox::evaluateEntry() {
    scenario->resetScenario();
    string oldname=scenario->mapname;
    if (scenario->loadMap(einput[0])!=0) {
        cout << "Map loading failed!" << endl;
        if (scenario->loadMap(oldname)!=0) {
            cout << "Couldn't even load the original map!" << endl;
            startScreen();
        }
    }
    editor->closeBox();
}

/* TODO: support not just image bgs
   TODO: support additional header options */
NewMapBox::NewMapBox(Sint16 x, Sint16 y): TextInputBox(x,y) {
    title="New Map"; 
    centered=false; 
    etitles.push_back("Map file: "); einput.push_back("newmap.cfg");
    etitles.push_back("Background image: "); einput.push_back(scenario->bgimage);
    currententry=0;
    update();
}

void NewMapBox::evaluateEntry() {
    scenario->resetScenario();
    ParameterMap bg_parameters;
    bg_parameters["image"]=einput[1];
    scenario->mapname=einput[0];
    scenario->newMap(bg_parameters);
    scenario->reloadMap();
    editor->closeBox();
}

ObjectBox::ObjectBox(string name, Sint16 x, Sint16 y, ParameterMap& parameters): TextInputBox(x,y),
  objname(name) {
    title=name+" Properties";
    centered=false;
    currententry=0;
    ParameterMap::iterator it=parameters.begin();
    while (it!=parameters.end()) {
        etitles.push_back((*it).first);
        einput.push_back((*it).second);
        ++it;
    }
    update();
}

void ObjectBox::evaluateEntry() {
    editor->place_name=objname;
    editor->place_parameters.clear();
    for (Uint8 i=0; i<einput.size(); ++i) {
        if (!einput[i].empty()) editor->place_parameters[etitles[i]]=einput[i];
    }
    if (editor->move_object) { 
        delete editor->move_object;
        editor->move_start=false;
    }
    editor->move_object=scenario->pool->addObjectbyName(editor->place_name,-1000,-1000,editor->place_parameters,true);
    if (editor->move_object) {
        editor->move_start=true;
        editor->action_mouse_pressed[SDL_BUTTON_LEFT]=EDIT_PLACE_OBJECT;
        editor->closeBox();
    }
}
