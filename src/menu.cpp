#include "common.h"
#include "gfxeng.h"
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

TestMenu::TestMenu(): Menu() { init(); }
void TestMenu::act() { }
void TestMenu::init() {
    title="-== TEST MENU ==-";
    entries.resize(4);
    entries[0]="FPS: " + itos(gfxeng->getFPS());
    entries[1]="Start Game";
    entries[2]="Configuration";
    entries[3]="Quit";
}
