#ifndef _GFXENG_H
#define _GFXENG_H 1

#define BAR_HEIGHT      138
#define ICON_SIZE       46
#define DFONT           40

/** \brief Graphics engine

    Draws each object in the pool and optionally a player bar and/or
    frames per seconds. Always run time resizing and fullscreen switching.
*/
class GraphicsEngine {
    public:
        GraphicsEngine();
        ~GraphicsEngine();
        /// Runs SDL_SetVideoMode with the new parameters.
        void resize(Uint16 width, Uint16 height);
        ///\brief update the scene in a running scenario
        ///
        /// If a scenario is running this draws the background and all objects
        /// in the pool. If the game is paused, only the player bar is drawn.
        /// Optionally also the player bar and/or the current frames per second
        /// are displayed.
        /// \param insist True if the objects should be drawn even if the game
        ///        is paused
        void renderScene(bool insist=false);
        void togglePlayerBar();
        void toggleFPS();
        void toggleFullScreen();
        ///\brief draw the menu
        ///
        /// If no scenario is running or the scenario was interrupted this will
        /// draw the menu.
        void drawMenu();
        void updateMenu() {
            menu_done=false;
        }
        void setMenuBG(SDL_Surface* menu_background=NULL);
        void resetMenuBG();
        Uint16 getFPS() {
            if (show_fps) return currentfps;
            else return 0;
        }
    private:
        Uint32 rmask;
        Uint32 gmask;
        Uint32 bmask;
        Uint32 amask;
        inline SDL_Rect clipToBG(SDL_Rect dest) const;
        /// updates backpos and returns the new shift vector (ignore w,h)
        inline SDL_Rect setShift(SDL_Rect center);
        /// simple vector addition, should be replaced by operator
        inline SDL_Rect* shiftMapArea(SDL_Rect& area, const SDL_Rect& shift);
        /// draw player bar
        inline void drawPlayerBar();
        /// currently visible part of the map area
        SDL_Rect vis_map;
        /// main screen
        SDL_Surface* screen;
        /// menu background
        SDL_Surface* menubg;
        /// sets the menu background while playing a scenario
        inline void setGameMenuBG();
        /// player bar
        SDL_Rect bar;
        /// symbol for one life of a player
        Animation* lifeimage;
        //fps stuff
        Uint16 Dfps,Dframes,currentfps,tcurrent;
        //for drawMenu
        bool menu_done;
        //visual flags
        bool show_bar;
        bool show_fps;
        bool fullscreen;
};

#endif
