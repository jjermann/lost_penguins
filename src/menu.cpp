#include "common.h"
#include "scenario.h"
#include "anim.h"
#include "gfxeng.h"
#include "sfxeng.h"
#include "menu.h"

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
            gfxeng->renderScene(true);
            scenario->anim->resetTime();
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
    entries.resize(2);
    update();
}
void ConfigMenu::act() {
    switch (currententry) {
    case 0: {
        gfxeng->toggleFPS();
        update();
        gfxeng->updateMenu();
        break;
    }
    case 1: {
        gfxeng->togglePlayerBar();
        gfxeng->renderScene(true);
        update();
        gfxeng->updateMenu();
        break;
    }
    default: {
        break;
    }
    }
}
void ConfigMenu::update() {
    entries[0]="Show FPS: "+string((gfxeng->show_fps) ? "ON" : "OFF");
    entries[1]="Show Player Bar: "+string((gfxeng->show_bar) ? "ON" : "OFF");
}

