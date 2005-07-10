#ifndef DEF_ANIMATION_H
#define DEF_ANIMATION_H 1

/** \brief A container for frames.

    Animation format: a horizontal strip (image) with (num) frames
    \todo Change this to an offset matrix or something similar or give each row a different animation
*/
class Animation {
    public:
        /// \brief Initialize the animation
        /// \param image Image used for the animation
        /// \param max_num Number of frames (will devide the image horizontally accordingly)
        /// \param total_time Duration of the animation, if set to 0 assume it's one still image
        /// \param once If true it run the animation only once when called by \fn start()
        Animation(SDL_Surface* image, Uint16 max_num=1, Uint16 total_time=0, bool once=false);
        /// \brief Initialize the animation
        /// \param width Image width used from the left side
        /// \param image Image used for the animation
        /// \param max_num Number of frames (will devide the image horizontally accordingly)
        /// \param total_time Duration of the animation, if set to 0 assume it's one still image
        /// \param once If true: Run the animation only once when called by \fn start()
        Animation(Uint16 width, SDL_Surface* image, Uint16 max_num=1, Uint16 total_time=0, bool once=false);
        ~Animation();
        /// Updates a running animation and stops it if necessary
        /// return True if the animation is still running
        /// \todo This should be more advanced
        bool updateAnim(Uint16 dt);
        /// Updates the frame status of a frame series (usually when not running)
        /// return Current frame
        const Frame& setFrame(Uint16 nr);
        /// Starts an (usually non running) animation
        void start() {
            num=0;
            tcurrent=0;  
            running=true;
        }
        const Frame& getFrame() const {
            return frame;
        }
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
        /// return True if the animation is running
        bool isRunning() const {
            return running;
        }   
    private:
        /// Frame pointer
        Frame frame;
        /// Number of frames
        Uint16 size;
        /// Total time (in ms)
        Uint16 time;  
        /// Frame size
        Uint16 w;
        Uint16 h;
        /// Current frame number
        Uint16 num;
        /// Current time position (in ms)
        Uint16 tcurrent;
        /// frame series or running?
        bool running;
        /// play the animation only once?
        bool once;
};

#endif
