#include "common.h"
#include "scenario.h"
#include "physics.h"
#include "gfxeng.h"
#include "sfxeng.h"
#include "menu.h"

Menu* setMenu(Menu* newmenu) {
    if (menu) gfxeng->update(UPDATE_MENU);
    else gfxeng->update(UPDATE_ALL);
    newmenu->setLast(menu);
    return menu=newmenu;   
}
 
Menu* closeMenu() {
    gfxeng->update(UPDATE_MENU);
    if (menu) {
        Menu* tmp=menu->getLast();
        delete menu;
        if (!tmp) gfxeng->update(UPDATE_ALL);
        return menu=tmp;
    } else {
        gfxeng->update(UPDATE_ALL);
        return NULL;
    }
}
 
void closeMenus() {
    while (menu) { 
        closeMenu();
    }
}

Menu::Menu():
  title("MENU"),
  last(NULL),
  currententry(0),
  //don't forget the "::"
  font(::font),
  font_title(font),
  font_high(font2) {
}
Menu::~Menu() { }
void Menu::increaseEntry(bool forward) {
    if (forward) {
        if (currententry<(entries.size()-1)) currententry++;
    } else {
        if (currententry>0) currententry--;
    }
}


StartMenu::StartMenu(): Menu() {
    title="-== MAIN MENU ==-";
    entries.resize(4);
    update();
}
void StartMenu::act() {
    switch (currententry) {
    case 0: {
        if ((config.map!="") && (scenario->loadMap(config.map)==0)) {
            closeMenu();
            cout << "Starting game...\n" << endl;
        } else {
            cout << "Select a valid map first...\n" << endl;
        }
        break;
    }
    case 1: {
        setMenu(new MapMenu());
        break;
    }
    case 2: {
        setMenu(new ConfigMenu());
        break;
    }
    case 3: {
        quitGame(0);
        break;
    }
    default: {
        break;
    }
    }
}
void StartMenu::update() {
    entries[0]="Start Game";
    entries[1]="Map selection";
    entries[2]="Configuration";
    entries[3]="Quit";
}


GameMenu::GameMenu(): Menu() {
    title="-== GAME MENU ==-";
    entries.resize(3);
    update();
}
void GameMenu::act() {
    switch (currententry) {
    case 0: {
        if (!closeMenu()) {
            gfxeng->update(UPDATE_ALL);
            sfxeng->resumeMusic();
        }
        break;
    }
    case 1: {
        setMenu(new ConfigMenu());
        break;
    }
    case 2: {
        quitGame(0);
        break;
    }
    default: {
        break;
    }
    }
}
void GameMenu::update() {
    entries[0]="Resume";
    entries[1]="Configuration";
    entries[2]="Quit";
}


MapMenu::MapMenu(): Menu() {
    title="-== MAP SELECTION ==-";
//TODO: add common entries, check directory for map files, entries.push_back(name);
    update();
}
void MapMenu::act() {
//TODO: if (currententry>C) open entries[currententry] + .cfg...
}
void MapMenu::update() {
}


ConfigMenu::ConfigMenu(): Menu() {
    title="-== CONFIGURATION MENU ==-";
    entries.resize(3);
    update();
}
void ConfigMenu::act() {
    switch (currententry) {
    case 0: {
        setMenu(new GraphicConfigMenu());
        break;
    }
    case 1: {
        setMenu(new KeyConfigMenu());
        break;
    }
    default: {
        break;
    }
    }
}
void ConfigMenu::update() {
    entries[0]="Graphic settings";
    entries[1]="Keyboard settings";
    entries[2]="Save settings";
}


KeyConfigMenu::KeyConfigMenu(): Menu() {
    title="-== KEYBOARD SETTINGS ==-";
    entries.resize(11);
    update();
}
void KeyConfigMenu::act() { }
void KeyConfigMenu::update() {
    entries[0]="Move left:  [" + string(SDL_GetKeyName(config.keybind[KEY_LEFT])) + "]";
    entries[1]="Move right:  [" + string(SDL_GetKeyName(config.keybind[KEY_RIGHT])) + "]";
    entries[2]="Move up:  [" + string(SDL_GetKeyName(config.keybind[KEY_UP])) + "]";
    entries[3]="Move down:  [" + string(SDL_GetKeyName(config.keybind[KEY_DOWN])) + "]";
    entries[4]="Special 1:  [" + string(SDL_GetKeyName(config.keybind[KEY_SP1])) + "]";
    entries[5]="Special 2:  [" + string(SDL_GetKeyName(config.keybind[KEY_SP2])) + "]";
    entries[6]="Activate:  [" + string(SDL_GetKeyName(config.keybind[KEY_ACT])) + "]";
    entries[7]="Use item:  [" + string(SDL_GetKeyName(config.keybind[KEY_USE])) + "]";
    entries[8]="Switch player:  [" + string(SDL_GetKeyName(config.keybind[KEY_SWITCH])) + "]";
    entries[9]="Drop item:  [" + string(SDL_GetKeyName(config.keybind[KEY_DROP])) + "]";
    entries[10]="Pause game:  [" + string(SDL_GetKeyName(config.keybind[KEY_PAUSE])) + "]";
}


GraphicConfigMenu::GraphicConfigMenu(): Menu() {
    title="-== GRAPHIC SETTINGS ==-";
    entries.resize(4);
    update();
}
void GraphicConfigMenu::act() {
    switch (currententry) {
    case 1: {
        gfxeng->toggleFullScreen();
        gfxeng->update(UPDATE_ALL);
        break;
    }
    case 2: {
        gfxeng->toggleFPS();
        gfxeng->update(UPDATE_MENU);
        break;
    }
    case 3: {
        gfxeng->togglePlayerBar();
        gfxeng->update(UPDATE_ALL);
        break;
    }
    default: {
        break;
    }
    }
    update();
}
void GraphicConfigMenu::update() {
    entries[0]="Resolution:  " + itos(gfxeng->screen->w) + " x " + itos(gfxeng->screen->h) + "  (" + itos(config.bpp) + " bpp)";
    entries[1]="Fullscreen:  " + string((gfxeng->fullscreen) ? "ON" : "OFF");
    entries[2]="Show FPS: "+string((gfxeng->show_fps) ? "ON" : "OFF");
    entries[3]="Show Player Bar: "+string((gfxeng->show_bar) ? "ON" : "OFF");
}
