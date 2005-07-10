#ifndef DEF_MENU_H
#define DEF_MENU_H 1

/** \brief abstract menu base class

    Abstract base class for menus. The main part is in the constructor and in the
    virtual function act(), overload it in the child classes. Only the base class
    is needed to draw the menu in gfxeng. The background depends from where the
    menu is called.
*/
class Menu {
    friend class GraphicsEngine;
    public:
        Menu();
        virtual ~Menu();
        /// Name of the menu
        string title;
        /// Main menu function, depends on currententry (changed by input), overload this.
        virtual void act() = 0;
        void setLast(Menu* lastmenu) {
            last=lastmenu;
        }
        Menu* getLast() {
            return last;
        }
        Uint8 getSize() {
            return entries.size();
        }
        void increaseEntry(bool forward=true);
    protected:
        /// Pointer to the last used menu. NULL if there is no last menu.
        Menu* last;
        /// Currently selected menu entry
        Uint8 currententry;
        /// List of all Menu entry names
        std::vector<string> entries;
        /// Font for non highlighted menu entries
        Font* font;
        /// Font for the menu title
        Font* font_title;
        /// Font for the selected menu entry
        Font* font_high;
};

class StartMenu : public Menu {
    public:
        StartMenu();
        virtual void act();
    private:
        /// Helper function
        void update();
};

class GameMenu : public Menu {
    public:
        GameMenu();
        virtual void act();
    private:
        /// Helper function
        void update();
};

class EditMenu : public Menu {
    public:
        EditMenu();
        virtual void act();
    private:
        /// Helper function
        void update();
};

class MapMenu : public Menu {
    public:
        MapMenu();
        virtual void act();
    private:
        /// Helper function
        void update();
};

class ConfigMenu : public Menu {
    public:
        ConfigMenu();
        virtual void act();
    private:
        /// Helper function
        void update();
};

class GraphicConfigMenu : public Menu {
    public:
        GraphicConfigMenu();
        virtual void act();
    private:
        /// Helper function
        void update();
};

class KeyConfigMenu : public Menu {
    public:
        KeyConfigMenu();
        virtual void act();
    private:
        /// Helper function
        void update();
};

#endif
