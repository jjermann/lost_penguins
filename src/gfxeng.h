#ifndef _GFXENG_H
#define _GFXENG_H 1

#define BAR_HEIGHT      138
#define ICON_SIZE       46

class GraphicsEngine {
    public:
        GraphicsEngine();
        ~GraphicsEngine();
        //drawing
        void renderScene(bool insist=true);
        void togglePlayerBar();
        void toggleFPS();
    private:
        inline SDL_Rect clipToBG(SDL_Rect dest) const;
        //updates backpos and returns the new shift vector (ignore w,h)
        inline SDL_Rect setShift(SDL_Rect center);
        //simple vector addition, should be replaced by operator
        inline SDL_Rect* shiftMapArea(SDL_Rect& area, const SDL_Rect& shift);
        //draw player bar
        inline void drawPlayerBar();
        //currently visible part of the maparea
        SDL_Rect backpos;
        //main screen
        SDL_Surface* screen;
        //player bar
        SDL_Rect bar;
        //player bar
        bool pbar;
        Animation* lifeimage;
        //fps stuff
        Uint16 Dfps,Dframes,currentfps,tcurrent;
        bool show_fps;
};

#endif
