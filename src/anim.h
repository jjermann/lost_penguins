#ifndef _ANIM_H_
    #define _ANIM_H_
#endif

//Animation format: a horizontal strip (image) with (num) frames
//Eventually change this to a matrix of frames: each row a different animation
class Animation {
    public:
        Animation(SDL_Surface* image, Uint16 max_num=1, Uint16 total_time=0, bool once=false);
        Animation(Uint16 width, SDL_Surface* image, Uint16 max_num=1, Uint16 total_time=0, bool an_once=false);
        ~Animation();
        //update a running animation, returns true if the animation is running
        bool updateAnim(Uint16 dt);
        //update frame status of a frame series (not running)
        const Frame& setFrame(Uint16 nr);
        void start() {
            num=0;
            tcurrent=0;
            running=true;
        }
        //return current frame
        const Frame& getFrame() const {
            return frame;
        }
        //return frame by number
        Frame getFrame(Uint16 nr) const {
            Frame newframe=frame;
            newframe.pos.x=nr*w;
            return newframe;
        }
        Uint16 getWidth() const {
            return w;
        }
        Uint16 getHeight() const {
            return h;
        }
        bool isRunning() const {
            return running;
        }
    private:
        //Frame pointer
        Frame frame;
        //Number of frames
        Uint16 size;
        //Total time (in ms)
        Uint16 time;
        //Frame size
        Uint16 w;
        Uint16 h;
        //Current frame number
        Uint16 num;
        //Current time position (in ms)
        Uint16 tcurrent;
        //frame series or running?
        bool running;
        //play the animation only once?
        bool once;
};

class AnimHandler {
    public:
        AnimHandler();
        ~AnimHandler();
        //run animations
        void runAnims();
        Uint16 resetTime();
    private:
        Uint16 tstart, tcurrent, dt;
};
