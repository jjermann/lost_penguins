#ifndef _GFXENG_H_
    #define _GFXENG_H_
#endif

#define BAR_HEIGHT      138
#define ICON_SIZE       46

class GraphicsEngine {
    public:
        GraphicsEngine();
        ~GraphicsEngine();
        //drawing
        void renderScene(bool insist=true);
        void toggleVikingBar();
    private:
        inline SDL_Rect clipToBG(SDL_Rect dest) const;
        //updates backpos and returns the new shift vector (ignore w,h)
        inline SDL_Rect setShift(SDL_Rect center);
        //simple vector addition, should be replaced by operator
        inline SDL_Rect* shiftMapArea(SDL_Rect& area, const SDL_Rect& shift);
        //draw viking bar
        inline void drawVikingBar();
        //currently visible part of the maparea
        SDL_Rect backpos;
        //main screen
        SDL_Surface* screen;
        //viking bar
        SDL_Rect bar;
        //viking bar
        bool vbar;
        Animation* lifeimage;
        //fps stuff
        Uint16 Dfps,Dframes,currentfps,tcurrent;
};
