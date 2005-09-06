#ifndef DEF_GFXENG_H
#define DEF_GFXENG_H 1

#define BAR_HEIGHT      138
#define ICON_SIZE       46
#define DFONT           10

#define UPDATE_NOTHING  0x00
#define UPDATE_BAR      0x01
#define UPDATE_MENU     0x02
#define UPDATE_ALL      0x03

/** \brief Graphics engine

    Draws each object in the pool and optionally a player bar and/or
    frames per seconds. Always run time resizing and fullscreen switching.
*/
class GraphicsEngine {
    friend class GraphicConfigMenu;
    public:
        GraphicsEngine();
        ~GraphicsEngine();
        /// Update what should be drawn to the screen
        void update(Uint8);
        /// Update the screen drawings...
        void draw();
        /// Runs SDL_SetVideoMode with the new parameters.
        void resize(Uint16 width, Uint16 height);
        void togglePlayerBar();
        void toggleFPS();
        void toggleFullScreen();
        void setShowDebug();
        void unsetShowDebug();
        void setMenuBG(SDL_Surface* menu_background=NULL);
        const SDL_Rect& addShift(Sint16,Sint16);
        const SDL_Rect& setShift(Sint16,Sint16);
        const SDL_Rect& getShift() {
            return shift;
        }
        void drawRectangle(SDL_Rect rect, Uint8 border=1, Uint32 color=0);
    protected:
        /// main screen
        SDL_Surface* screen;
        /// currently visible part of the map area
        SDL_Rect vis_map;
        /// menu background
        SDL_Surface* menubg;
        /// player bar
        SDL_Rect bar;
        bool show_bar;
        bool show_fps;
        bool show_debug;
        //visual flags
        bool fullscreen;
        //update state
        Uint8 updatetype;
        // current shift
        SDL_Rect shift;
        /// symbol for one life of a player
        EmptyAnimationPtr lifeimage;
    protected:
        /// Draw the background and all objects in the pool. This is a very time
        /// consuming function...
        inline void drawScene();
        /// Draw player bar
        inline void drawPlayerBar();
        /// Draw the frames per second
        inline void drawFPS();
        ///\brief draw the menu
        ///
        /// If no scenario is running or the scenario was interrupted this will
        /// draw the menu.
        inline void drawMenu();
        inline void drawBox();
        inline void resetMenuBG();
        inline SDL_Rect clipToBG(SDL_Rect dest) const;
        /// updates backpos and returns the new shift vector (ignore w,h)
        inline SDL_Rect setShift(SDL_Rect center);
        /// simple vector addition, should be replaced by operator
        inline SDL_Rect* shiftMapArea(SDL_Rect& area, const SDL_Rect& shift);
        /// sets the menu background while playing a scenario
        inline void setGameMenuBG();
};

#endif
